#include <sim/insn.hpp>
#include <variant>
#include <sim/util.hpp>
#include <algorithm>

bool sim::ready(sim::live_insn i) {
    return std::all_of (
        i.operands.begin(),
        i.operands.end(),
        [](sim::future<sim::word> operand) { return static_cast<bool>(operand); }
    );
}