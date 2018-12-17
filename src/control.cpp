#include <sim/control.hpp>
#include <sim/operand.hpp>
#include <sim/util.hpp>
#include <sim/future.hpp>
#include <fmt/format.h>
#include <sim/insn.hpp>
#include <algorithm>
#include <iterator>

sim::future<sim::word> sim::resolve_op(encoded_operand operand) {
    return std::visit(match {
        [](sim::word immediate) { return sim::ready(immediate); },
        [](sim::areg from_reg) {
            if (auto fut_it = rat.find(from_reg); fut_it == rat.end()) {
                return sim::ready(crf[from_reg]);
            } else {
                return fut_it->second;
            }
        }
    }, operand);
}

// For now always predict branch will be taken
void sim::speculate(const sim::static_insn& branch) {
    sim::pc = sim::resolve_op(*branch.branches_to);
}

void sim::fetch() {
    if (pc) {
        for(int i = 0; i < sim::pipeline_width
                    && !decode_buffer.full()
                    && *sim::pc < static_cast<sim::addr_t>(sim::main_memory.size()); i++) {
            if (std::visit( match {
                [](sim::encoded_insn encoded) {
                    sim::decode_buffer.push_back({*sim::pc, encoded});
                    sim::pc = sim::ready(*sim::pc + 1);
                    return false;
                },
                [](sim::word data) {
                    return true;
                }
            }, sim::main_memory[*sim::pc])) {
                break;
            };
        }
    }
}

void sim::decode() {
    for(int i = 0; i < sim::pipeline_width && !decode_buffer.empty() && !insn_queue.full(); i++) {
        static_insn decoded;
        *static_cast<sim::encoded_insn*>(&decoded) = std::get<sim::encoded_insn>(decode_buffer.front().second);
        decoded.address = decode_buffer.front().first;
        insn_queue.push_back(decoded);
        decode_buffer.pop_front();
        if (decoded.branches_to) {
            decode_buffer.clear();
            sim::speculate(decoded);
            break;
        }
    }
}

void sim::issue() {
    while(!insn_queue.empty() && !rob.full()) {
        if (auto rs = std::find_if(sim::rs_slots.begin(),
                                   sim::rs_slots.end(),
                                   [](sim::reservation_station_slot& rs) { return !rs.has_value(); });
            rs != sim::rs_slots.end())
            {
            sim::static_insn statik = insn_queue.front();
            insn_queue.pop_front(); 

            sim::live_insn live;
            live.opcode = statik.opcode;
            std::transform (
                statik.operands.begin(), statik.operands.end(),
                std::back_inserter(live.operands),
                sim::resolve_op
            );
            live.destination = statik.destination;
            live.address = statik.address;

            switch (live.opcode) {
                case opcode::add:
                case opcode::ldw: {
                    sim::promise<sim::word> result;
                    *rs = sim::reservation_station{live, result};
                    sim::rat.insert_or_assign(*live.destination, result.anticipate());
                    sim::rob.push_back ({
                        live,
                        sim::writeback{result.anticipate(), *live.destination}
                    });
                    break;
                }
                case opcode::stw: {
                    // Don't need a promise because this doesn't writeback to registers
                    break;
                }
                case opcode::jneq: {
                    throw std::runtime_error("Can't handle branches yet LOL");
                    break;
                }
                case opcode::halt: {
                    break;
                }
                default: throw std::runtime_error("invalid opcode");
            }
        } else {
            break;
        }
    }
}

void sim::execute() {
    // Dispatch
    for (sim::reservation_station_slot& slot : sim::rs_slots) {
        if (slot) {
            if (sim::ready(slot->waiting)) {
                fmt::print("  ready: {}\n", slot->waiting);
                if (auto eu_it = std::find_if(sim::execution_units.begin(),
                                              sim::execution_units.end(),
                                              [&] (const sim::execution_unit& eu) { return eu.can_start(slot->waiting); });
                    eu_it != sim::execution_units.end())
                    {
                    eu_it->start(slot->waiting, slot->broadcast);
                    slot.reset();
                }
            } else {
                fmt::print("waiting: {}\n", slot->waiting);
            }
        }
    }
    // Work
    for (auto& eu : sim::execution_units) {
        eu.work();
    }
}

void sim::commit() {
    
}

void sim::tick() {
    sim::commit();
    sim::execute();
    sim::issue();
    sim::decode();
    sim::fetch();
    sim::t++;
}