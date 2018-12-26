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

void sim::execute() {
    for (auto& eu : sim::execution_units) {
        eu.dispatch();
    }
    for (auto& eu : sim::execution_units) {
        eu.work();
    }
    for (auto& eu : sim::execution_units) {
        eu.broadcast();
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