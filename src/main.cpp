#include <sim/control.hpp>
#include <algorithm>
#include <fmt/format.h>
#include <sim/reorder_buffer.hpp>
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
}

int main() {
    sim::crf[sim::areg::g0] = 1;
    sim::crf[sim::areg::g1] = 2;
    sim::crf[sim::areg::g2] = 0;
    sim::crf[sim::areg::g3] = 0;
    sim::crf[sim::areg::g4] = 6;
    sim::main_memory[0x6] = 42;
    sim::pc = sim::ready(0x0);
    auto mem = sim::main_memory.begin();
    *mem++ = sim::encoded_insn {sim::opcode::add, {sim::areg::g0, sim::areg::g1, sim::areg::g2}};
    *mem++ = sim::encoded_insn {sim::opcode::stw, {17, 6}};
    *mem++ = sim::encoded_insn {sim::opcode::ldw, {sim::areg::g4, sim::areg::g4}};
    *mem++ = sim::encoded_insn {sim::opcode::add, {sim::areg::g0, sim::areg::g2, sim::areg::g3}};
    *mem++ = sim::encoded_insn {sim::opcode::add, {sim::areg::g2, sim::areg::g3, sim::areg::g0}};
    *mem++ = sim::encoded_insn {sim::opcode::add, {sim::areg::g4, sim::areg::g4, sim::areg::g4}};
    *mem++ = sim::encoded_insn {sim::opcode::halt};

    
    try {
        while (true) {
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