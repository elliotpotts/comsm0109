#include <sim/execution_unit.hpp>
#include <fmt/format.h>

static bool alu_oc_pred(sim::opcode oc) {
    return oc == sim::opcode::add;
}
static int alu_start(sim::live_insn) {
    return 0;
}
static void alu_finish(sim::live_insn i, std::variant<sim::promise<sim::word>, sim::promise<bool>, std::nullopt_t> result) {
    std::get<sim::promise<sim::word>>(result).fulfil(*i.operands[0] + *i.operands[1]);
}

bool sim::execution_unit::can_start(sim::live_insn i) const {
    return !executing.has_value() && handles_opcode(i.opcode);
}
void sim::execution_unit::start(sim::live_insn i, sim::magic_promise bcast) {
    executing = i;
    broadcast = bcast;
    ticks_left = starter(i);
}
void sim::execution_unit::work() {
    if (executing) {
        ticks_left--;
        if (ticks_left <= 0) {
            finisher(*executing, broadcast);
            fmt::print("finished executing {}\n", *executing);
            executing.reset();
            broadcast = std::nullopt;
            ticks_left = -1;
        }
    }
}

sim::execution_unit sim::make_alu() {
    return {alu_oc_pred, alu_start, alu_finish};
}