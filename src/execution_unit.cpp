#include <sim/execution_unit.hpp>
#include <fmt/format.h>
#include <sim/control.hpp>

void sim::execution_unit::dispatch() {
    if (executing) return;
    for (std::optional<std::unique_ptr<sim::reservation>>& slot : sim::res_stn) {
        if (slot && (**slot).ready()) {
            executing = std::move(*slot);
            slot.reset();
            ticks_left = executing->worktime();
            break;
        }
    }
}

void sim::execution_unit::work() {
    if (!executing) return;
    ticks_left--;
}

void sim::execution_unit::broadcast() {
    if (!executing) return;
    if (ticks_left <= 0) {
        executing->execute();
        executing.reset();
    }
}

/*
sim::alu::alu() : ticks_left{0} {
}

void sim::alu::dispatch() {
    if (working) return;
    for (std::optional<sim::reservation>& slot : sim::res_stn) {
        if (slot && slot->ready() && slot->opcode == sim::opcode::add) {
            working = *slot;
            ticks_left = 1;
            slot.reset();
        }
    }
}

void sim::alu::work() {
    ticks_left--;
    if (ticks_left == 0) {
        
    }
}

class alu : public execution_unit {
    int ticks_left;
    std::optional<reservation> working;
public:
    alu();
    virtual void dispatch() override;
    virtual void work() override;
    virtual void broadcast() override;
};

bool sim::execution_unit::can_start(const sim::reservation& rs) const {
    return !computed.has_value() && mask(rs.opcode);
}
void sim::execution_unit::start(const reservation& rs) {
    oc = rs.opcode;
    std::transform (
        rs.operands.cbegin(),
        rs.operands.cend(),
        std::back_inserter(operands),
        [](const sim::future<sim::word>& fut) { return *fut; }
    );
    computed = rs.result;
    ticks_left = time(rs.opcode, operands);
}
void sim::execution_unit::work() {
    if (oc) {
        ticks_left--;
        if (ticks_left <= 0) {
            execute(*oc, operands, *computed);
            oc.reset();
            operands.clear();
            computed.reset();
            ticks_left = -1;
        }
    }
}

static bool alu_mask(sim::opcode oc) {
    return oc == sim::opcode::add;
}
static int alu_time(sim::opcode oc, std::vector<sim::word> operands) {
    return 0;
}
static void alu_execute(sim::opcode opcode, std::vector<sim::word> operands, sim::promise<sim::word> result) {
    result.fulfil(operands[0] + operands[1]);
}
sim::execution_unit sim::make_alu() {
    return {alu_mask, alu_time, alu_execute};
}*/