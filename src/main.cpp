#include <sim/control.hpp>
#include <algorithm>
#include <fmt/format.h>
#include <sim/reorder_buffer.hpp>
#include <sim/lsq.hpp>
#include <variant>

void summarise() {
    fmt::print("------------ at t = {}: --------------------------\n", sim::t);
    fmt::print(" {:2}/{} instructions in decode buffer\n", sim::decode_buffer.size(), sim::decode_buffer.capacity());
    fmt::print(" {:2}/{} instructions in instruction queue\n", sim::insn_queue.size(), sim::insn_queue.capacity());
    fmt::print(" {:2}/{} non-empty reservation stations\n",
        std::count_if(sim::res_stn.begin(),
                      sim::res_stn.end(),
                      [](const std::optional<auto>& rs) { return rs.has_value(); }),
        sim::res_stn.size());
    fmt::print(" {:2}/{} awaiting commitments in reorder buffer\n", sim::rob.size(), sim::rob.capacity());
    for(const sim::commitment& commit : sim::rob) {
        fmt::print("    {}\n", commit);
    }
    fmt::print(" {:2}/{} items in load store queue\n", sim::lsq.size(), sim::lsq.capacity());
    for(const sim::load_store& ls : sim::lsq) {
        fmt::print("    {}\n", ls);
    }
}

int main() {
    sim::execution_units.push_back(std::make_unique<sim::alu>());
    sim::execution_units.push_back(std::make_unique<sim::alu>());
    sim::execution_units.push_back(std::make_unique<sim::alu>());
    sim::execution_units.push_back(std::make_unique<sim::lunit>());

    sim::pc = 0x0;
    auto mem = sim::main_memory.begin();
    // calculate 2*6 by repeated addition
    *mem++ = sim::add {0, 0, sim::areg::g4 }; // nop
    *mem++ = sim::add {0, 0, sim::areg::g0 }; // g0 = 0; loop counter
    *mem++ = sim::add {0, 0, sim::areg::g1 }; // g1 = 0; comparison result
    *mem++ = sim::add {0, 0, sim::areg::g2 }; // g2 = 2; accumulator
    // do {
        // g2 = g2 + 2
        *mem++ = sim::add {sim::areg::g2,  2, sim::areg::g2};
        // g0 = g0 + 1
        *mem++ = sim::add {sim::areg::g0,  1, sim::areg::g0};
        // g1 = g0 < 6 ? -1 : 0
        *mem++ = sim::cmp {sim::areg::g0,  6, sim::areg::g1};
        *mem++ = sim::jeq {sim::areg::g1, -1, -3};
    // } while (g1 == -1)
    *mem++ = sim::halt {};

    int max_cycles = 100;
    try {
        while (true && sim::t < max_cycles) {
            summarise();
            sim::tick();
        }
    } catch (const sim::trap& t) {
        fmt::print("\n\n\n EXECUTION HALTED\n");
        for(auto pair : sim::crf) {
            fmt::print("   {} = {}\n", pair.first, pair.second);
        }
    }
    
    return 0;
}