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

std::vector<std::optional<sim::reservation>>::iterator sim::find_reservation() {
    return std::find_if(sim::res_stn.begin(),
                        sim::res_stn.end(),
                        [](std::optional<sim::reservation>& rs) { return !rs.has_value(); });
}

void sim::fetch() {
    if (pc) {
        while (!decode_buffer.full()
            && *sim::pc < static_cast<sim::addr_t>(sim::main_memory.size())) {
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
    while (!decode_buffer.empty() && !insn_queue.full()) {
        std::unique_ptr<sim::insn> decoded = sim::decode_at (
            std::get<sim::encoded_insn>(decode_buffer.front().second),
            decode_buffer.front().first
        );
        //fmt::print("  decoding: {}\n", decoded.head);
        insn_queue.push_back(std::move(decoded));
        decode_buffer.pop_front();
    }
}

void sim::issue() {
    while(!insn_queue.empty() && !rob.full()) {
        if (sim::insn_queue.front()->try_issue()) {
            insn_queue.pop_front();
        } else {
            break;
        }
    }
}

void sim::execute() {/*
    // Work
    for (auto& eu : sim::execution_units) {
        eu.work();
    }
    // Dispatch
    for (std::optional<sim::reservation>& slot : sim::res_stn) {
        if (slot) {
            if (sim::ready(slot->waiting)) {
                if (auto eu_it = std::find_if(sim::execution_units.begin(),
                                              sim::execution_units.end(),
                                              [&] (const sim::execution_unit& eu) { return eu.can_start(slot->waiting); });
                    eu_it != sim::execution_units.end())
                    {
                    fmt::print("dispatching: {}\n", slot->waiting);
                    eu_it->start(slot->waiting, slot->promises);
                    slot.reset();
                } else {
                    fmt::print("no execution units available for {}\n", slot->waiting);
                }
            } else {
                fmt::print("    waiting: {}\n", slot->waiting);
            }
        } else {
            fmt::print("(rs empty)\n");
        }
    }*/
}

void sim::commit() {/*
    int commits_left = 6;
    while (!rob.empty() && commits_left > 0) {
        if (sim::ready(sim::rob.front().commit)) {
            commits_left--;
            reorder in_order = rob.front();
            fmt::print(" committing: {}\n", in_order.insn);
            rob.pop_front();
            std::visit( match {
                [](writeback wb) { sim::crf[wb.dest] = *wb.value; }, 
                [](store st) { sim::main_memory[*st.dest] = *st.value; },
                [](branch b) { throw std::runtime_error("Can't handle branch"); },
                [](halt h) { throw std::runtime_error("Halted"); },
                [](std::monostate) {}
            }, in_order.commit);
        } else {
            break;
        }
    }*/
}

void sim::tick() {
    sim::commit();
    sim::execute();
    sim::issue();
    sim::decode();
    sim::fetch();
    sim::t++;
}