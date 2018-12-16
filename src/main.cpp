#include <sim/control.hpp>
#include <algorithm>
#include <fmt/format.h>

void summarise() {
    fmt::print("{} instructions in decode buffer\n", sim::decode_buffer.size());
    fmt::print("{} instructions in instruction queue\n", sim::insn_queue.size());
    fmt::print("{} non-empty reservation stations\n",
        std::count_if(sim::reservation_stations.begin(),
                      sim::reservation_stations.end(),
                      [](const sim::reservation_station& rs) { return !rs.empty(); }));
    fmt::print("{} instructions awaiting commit in reorder buffer\n", 0);
    fmt::print("-------------------------------\n");
}

int main() {
    /*sim::main_memory[0x00] = 1;
    sim::main_memory[0x01] = 2;
    sim::main_memory[0x02] = 3;
    sim::main_memory[0x03] = 4;
    sim::main_memory[0x04] = 5;
    sim::main_memory[0x05] = 6;
    sim::main_memory[0x06] = 0;
    sim::main_memory[0x07] = 0;
    sim::main_memory[0x08] = 0;
    sim::main_memory[0x09] = sim::encoded_insn {sim::opcode::mov, {{sim::areg::g0}} };*/
    sim::main_memory[0x00] = sim::encoded_insn {sim::opcode::mov, {{sim::areg::g0}}, sim::areg::g1};

    summarise();
    sim::tick();
    summarise();
    return 0;
}