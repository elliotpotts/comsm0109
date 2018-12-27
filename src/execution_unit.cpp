#include <sim/execution_unit.hpp>
#include <fmt/format.h>
#include <sim/control.hpp>
#include <unordered_set>
#include <iterator>
#include <variant>

void sim::alu::start() {
    if (executing) return;
    for (std::optional<sim::reservation>& slot : sim::res_stn) {
        if (slot && sim::ready(*slot)) {
            executing = std::move(*slot);
            slot.reset();
            ticks_left = sim::worktime(*executing);
            break;
        }
    }
}

void sim::alu::work() {
    if (!executing) return;
    ticks_left--;
}

void sim::alu::finish() {
    if (!executing) return;
    if (ticks_left <= 0) {
        sim::execute(*executing);
        executing.reset();
    }
}

void sim::lunit::start() {
    if (executing) return;
    auto ld_it = std::find_if (
        sim::lsq.begin(),
        sim::lsq.end(),
        [](const load_store& ls) {
            auto ld_ptr = std::get_if<load>(&ls);
            return ld_ptr && ld_ptr->addr && !(ld_ptr->loader || ld_ptr->data);
        }
    );
    if (ld_it != sim::lsq.end()) {
        load& ld = std::get<sim::load>(*ld_it);
        ld.loader = this;
        executing = &ld;
        ticks_left = 10;
    }
}

void sim::lunit::work() {
    if (!executing) return;
    ticks_left--;
}

// Loads are removed from the LSQ after execution.
// Stores are removed from the LSQ after commit.
void sim::lunit::finish() {
    if (!executing) return;
    if (ticks_left <= 0) {
        executing->data.fulfil(std::get<sim::addr_t>(sim::main_memory.at(*executing->addr)));
        executing->loader = nullptr;
    }
}

void sim::lunit::cancel() {
    executing = nullptr;
    ticks_left = -1;
}