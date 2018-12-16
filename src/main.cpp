#include <sim/control.hpp>
#include <algorithm>
#include <fmt/format.h>

void summarise() {
    fmt::print("------------ at t = {}: --------------------------\n", sim::t);
    fmt::print(" {:2}/{} instructions in decode buffer\n", sim::decode_buffer.size(), sim::decode_buffer.capacity());
    fmt::print(" {:2}/{} instructions in instruction queue\n", sim::insn_queue.size(), sim::insn_queue.capacity());
    fmt::print(" {:2}/{} non-empty reservation stations\n",
        std::count_if(sim::rs_slots.begin(),
                      sim::rs_slots.end(),
                      [](const sim::reservation_station_slot& rs) { return rs.has_value(); }),
        sim::rs_slots.size());
    fmt::print(" {:2}/{} instructions awaiting commit in reorder buffer\n", sim::rob.size(), sim::rob.capacity());
}

int main() {
    sim::crf[sim::areg::g0] = 1;
    sim::crf[sim::areg::g1] = 2;
    sim::pc = sim::ready(0x0);
    sim::main_memory[0] = sim::encoded_insn {sim::opcode::add, {{sim::areg::g0, sim::areg::g1}}, sim::areg::g2};

    for(int i = 0; i < 5; i++) {
        summarise();
        sim::tick();
    }
    return 0;
}