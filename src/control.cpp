#include <sim/control.hpp>
#include <sim/operand.hpp>
#include <sim/util.hpp>
#include <sim/future.hpp>
#include <fmt/format.h>

// TODO: branch prediction
void sim::fetch() {
    for(int i = 0; i < sim::pipeline_width
                && !decode_buffer.full()
                && sim::pc < static_cast<int>(main_memory.size()); i++) {
        fmt::print("Fetching from {:#x}: ", sim::pc);
        auto p_encoded = std::get_if<sim::encoded_insn>(&sim::main_memory[sim::pc]);
        if (p_encoded) {
            fmt::print("{}\n", p_encoded->opcode);
            sim::decode_buffer.push_back(*p_encoded);
            sim::pc++;
        } else {
            fmt::print("data.\n");
            break;
        }
    }
}

void sim::decode() {
    for(int i = 0; i < sim::pipeline_width && !decode_buffer.empty() && !insn_queue.full(); i++) {
        insn_queue.push_back(std::get<sim::encoded_insn>(decode_buffer.front()));
        decode_buffer.pop_front();
    }
}

void sim::issue() {
    while(!insn_queue.empty() && !rob.full()) {
        if(auto rs = std::find_if(reservation_stations.begin(),
                                  reservation_stations.end(),
                                  [](sim::reservation_station& rs) { return rs.empty(); });
           rs != reservation_stations.end())
            {
            sim::encoded_insn encoded = insn_queue.front();
            insn_queue.pop_front();

            // Create "in-flight" instruction using renamed operands
            sim::insn inflight;
            inflight.opcode = encoded.opcode;
            for(sim::encoded_operand op : encoded.operands) {
                std::visit(match {
                    [&](sim::areg src) {
                        if(auto renamed_it = rat.find(src); renamed_it == rat.end()) {
                            std::pair<decltype(rat)::iterator, bool> inserted = rat.insert({src, sim::immediate(crf[src])});
                            inflight.operands.push_back(inserted.first->second);
                        } else {
                            inflight.operands.push_back(renamed_it->second);
                        }
                    },
                    [&](sim::word imm) { inflight.operands.push_back(sim::immediate(imm)); }
                }, op);
            }

            // Add reorder buffer entry
            switch (inflight.opcode) {
                case opcode::mov:
                case opcode::ldw: {
                    sim::promise<sim::word>& result = rs->hold(inflight);
                    rat.insert_or_assign(*encoded.destination, result.anticipate());
                    rob.plan(sim::reg_write{result.anticipate(), *encoded.destination});
                    break;
                }
                case opcode::stw: {
                    break;
                }
                case opcode::jneq: {
                    sim::promise<bool>& taken = rs->hold_branch(inflight);
                    rob.plan(sim::branch{sim::immediate(0), taken.anticipate()});
                    break;
                }
                case opcode::halt: {
                    rob.plan(sim::halt{});
                    break;
                }
                default:
                    throw std::runtime_error("invalid opcode");
                    break;
            }
        } else {
            break;
        }
    }
}

void sim::execute() {
    fmt::print("executing\n");
}

void sim::commit() {
    fmt::print("committing\n");
}

void sim::tick() {
    sim::commit();
    sim::execute();
    sim::issue();
    sim::decode();
    sim::fetch();
}