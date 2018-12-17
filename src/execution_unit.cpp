#include <sim/execution_unit.hpp>
#include <fmt/format.h>

bool sim::execution_unit::can_start(sim::live_insn i) const {
    return !executing.has_value() && handles_opcode(i.opcode);
}
void sim::execution_unit::start(sim::live_insn i, std::vector<std::any> outputs) {
    executing = i;
    promises = outputs;
    ticks_left = starter(i);
}
void sim::execution_unit::work() {
    if (executing) {
        ticks_left--;
        if (ticks_left <= 0) {
            finisher(*executing, promises);
            fmt::print("finished executing {}\n", *executing);
            executing.reset();
            promises.clear();
            ticks_left = -1;
        }
    }
}

static bool alu_oc_pred(sim::opcode oc) {
    return oc == sim::opcode::add;
}
static int alu_start(sim::live_insn) {
    return 0;
}
static void alu_finish(sim::live_insn i, std::vector<std::any> results) {
    std::any_cast<sim::promise<sim::word>>(results[0]).fulfil(*i.operands[0] + *i.operands[1]);
}
sim::execution_unit sim::make_alu() {
    return {alu_oc_pred, alu_start, alu_finish};
}

static bool lsu_oc_pred(sim::opcode oc) {
    return oc == sim::opcode::ldw;
}
static int lsu_start(sim::live_insn) {
    return 3;
}
static void lsu_finish(sim::live_insn, std::vector<std::any> results) {
}
sim::execution_unit sim::make_lsu() {
    return {lsu_oc_pred, lsu_start, lsu_finish};
}