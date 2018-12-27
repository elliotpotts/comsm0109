#include <sim/execution_unit.hpp>
#include <fmt/format.h>
#include <sim/control.hpp>
#include <unordered_set>
#include <iterator>
#include <variant>
#include <algorithm>

void sim::alu::cancel() {
    executing.reset();
    ticks_left = -1;
}
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
void sim::lunit::cancel() {
    executing = nullptr;
    ticks_left = -1;
}

void sim::lunit::start() {
    if (executing) return;
    auto ld_it = std::find_if (
        sim::lsq.begin(),
        sim::lsq.end(),
        [](const load_store& ls) {
            auto ld_ptr = std::get_if<load>(&ls);
            return ld_ptr && ld_ptr->addr && !ld_ptr->data && !ld_ptr->loader;
        }
    );
    if (ld_it != sim::lsq.end()) {
        load& ld = std::get<sim::load>(*ld_it);
        ld.loader = this;
        executing = &ld;
        ticks_left = 1;
    }
}
void sim::lunit::work() {
    if (!executing) return;
    ticks_left--;
}
void sim::lunit::finish() {
    if (!executing) return;
    if (ticks_left <= 0) {
        executing->data.fulfil(std::get<sim::addr_t>(sim::main_memory.at(*executing->addr)));
        executing->loader = nullptr;
        executing = nullptr;
    }
}

void sim::sunit::cancel() {
    executing = nullptr;
    ticks_left = -1;
}
void sim::sunit::start() {
    if (executing || sim::rob.empty()) return;
    if (auto st_ptr = std::get_if<store>(&sim::rob.front());
        st_ptr && st_ptr->data && st_ptr->addr && !st_ptr->storer)
        {
        st_ptr->storer = this;
        executing = st_ptr;
        ticks_left = 1;
    }
}
void sim::sunit::work() {
    if (!executing) return;
    ticks_left--;
}
void sim::sunit::finish() {
    if (!executing) return;
    if (ticks_left <= 0) {
        executing->committed = true;
        // remove corresponding lsq entry
        auto lsq_it = std::find_if (
            sim::lsq.begin(),
            sim::lsq.end(),
            [this](const sim::load_store& ls) {
                auto st_ptr = std::get_if<sim::store>(&ls);
                return st_ptr
                    && *st_ptr->addr == *executing->addr
                    && *st_ptr->data == *executing->data;
            }
        );
        if (lsq_it == sim::lsq.end()) {
            throw std::runtime_error("can't find lsq store corresponding to commit!\n");
        }
        sim::lsq.erase(lsq_it);
    }
}