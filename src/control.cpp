#include <sim/control.hpp>
#include <sim/operand.hpp>
#include <sim/util.hpp>
#include <sim/future.hpp>
#include <fmt/format.h>
#include <sim/insn.hpp>
#include <algorithm>
#include <iterator>
#include <variant>

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

void sim::fetch() {
    if (true) {
        while (!decode_buffer.full()
            && sim::pc < static_cast<sim::addr_t>(sim::main_memory.size())) {
            bool stop_fetching = false;
            std::visit ( match {
                [&](sim::word data) {
                    stop_fetching = true; // stop fetching
                },
                [&](sim::encoded_insn& encoded) {
                    // pre-decode to see if we hit a branch
                    if (auto br_ptr = std::get_if<sim::jeq>(&encoded); br_ptr) {
                        br_ptr->origin = sim::pc;
                        return std::visit( match {
                            [&](const sim::word offset) {
                                // TODO: improve branch prediction
                                if (offset < 0) {
                                    sim::pc += offset;
                                    br_ptr->predicted = true;
                                } else {
                                    sim::pc++;
                                    br_ptr->predicted = false;
                                }
                                sim::decode_buffer.push_back(encoded);
                            },
                            [&](const sim::areg reg) {
                                throw std::runtime_error("can't deal with reg dst");
                                stop_fetching = true; // stall until we have the address
                            }
                        }, br_ptr->offset);
                    } else {
                        sim::pc = sim::pc + 1;
                        sim::decode_buffer.push_back(encoded);
                    }
                }
            }, sim::main_memory[sim::pc]);
            if (stop_fetching) {
                break;
            };
        }
    }
}

void sim::decode() {
    while (!decode_buffer.empty() && !insn_queue.full()) {
        sim::insn decoded = std::move(decode_buffer.front());
        insn_queue.push_back(std::move(decoded));
        decode_buffer.pop_front();
    }
}

void sim::issue() {
    while(!insn_queue.empty() && !rob.full()) {
        if (sim::try_issue(insn_queue.front())) {
            insn_queue.pop_front();
        } else {
            break;
        }
    }
}

void sim::execute() {
    forward_stores();
    for (auto& eu : sim::execution_units) {
        eu->start();
    }
    for (auto& eu : sim::execution_units) {
        eu->work();
    }
    for (auto& eu : sim::execution_units) {
        eu->finish();
    }
}

void sim::commit() {
    int commits_left = sim::pipeline_width;
    while (!rob.empty() && commits_left > 0) {
        if (sim::ready(sim::rob.front())) {
            commits_left--;
            sim::commit(sim::rob.front());
            sim::rob.pop_front();
        } else {
            break;
        }
    }
}

void sim::tick() {
    sim::commit();
    sim::execute();
    sim::issue();
    sim::decode();
    sim::fetch();
    sim::t++;
}