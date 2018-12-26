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