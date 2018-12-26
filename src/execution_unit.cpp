#include <sim/execution_unit.hpp>
#include <fmt/format.h>
#include <sim/control.hpp>
#include <unordered_set>
#include <iterator>
#include <variant>
#include <boost/iterator/iterator_adaptor.hpp>

void sim::alu::start() {
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

void sim::alu::work() {
    if (!executing) return;
    ticks_left--;
}

void sim::alu::finish() {
    if (!executing) return;
    if (ticks_left <= 0) {
        executing->execute();
        executing.reset();
    }
}

void sim::forward_stores() {
    for (auto ld_it = sim::lsq.begin(); ld_it != sim::lsq.end(); ld_it++) {
        auto ld_ptr = std::get_if<sim::load>(&*ld_it);
        if (ld_ptr && ld_ptr->addr && !ld_ptr->data) {
            // Find a store with a matching address
            bool potential_conflict = false;
            auto fwdng_it = std::find_if (
                boost::make_reverse_iterator(ld_it),
                sim::lsq.rend(),
                [&](const load_store& ls) {
                    return std::visit( match {
                        [&](const store& st) {
                            // An unknown store address poses a conflict as
                            // any loads occurring aftwards could load from
                            // it.
                            if (!st.addr) potential_conflict = true;
                            return !potential_conflict
                                && st.addr
                                && *st.addr == *ld_ptr->addr
                                && st.data;
                        },
                        [&](const load&) { return false; }
                    }, ls);
                }
            );
            if (fwdng_it != sim::lsq.rend()) {
                if (ld_ptr->loader) {
                    ld_ptr->loader->cancel();
                }
                ld_ptr->data.fulfil(*std::get<store>(*fwdng_it).data);
            }
        }
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