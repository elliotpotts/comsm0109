#include <sim/execution_unit.hpp>
#include <fmt/format.h>
#include <sim/control.hpp>
#include <unordered_set>
#include <iterator>
#include <variant>
#include <algorithm>

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
bool sim::alu::busy() const {
    return executing.has_value();
}
void sim::alu::cancel() {
    executing.reset();
    ticks_left = -1;
}
void sim::alu::finish() {
    if (!executing) return;
    if (ticks_left <= 0) {
        sim::execute(*executing);
        executing.reset();
    }
}

#include <map>
#include <optional>
void sim::lunit::start() {
    if (executing) return;
    std::map<addr_t, std::optional<word>> stores; 
    for (load_store& ls : sim::lsq) {
        if (auto st_ptr = std::get_if<store>(&ls);
            st_ptr) {
            if (!st_ptr->addr) {
                return; // could cause conflict, don't start anything.
            } else if (st_ptr->data) {
                stores.insert_or_assign(*st_ptr->addr, *st_ptr->data);
            } else {
                stores.try_emplace(*st_ptr->addr);
            }
        } else {
            load& ld = std::get<load>(ls);
            if (ld.addr && !ld.data && !ld.loader) {
                if (auto st_it = stores.find(*ld.addr); st_it != stores.end()) {
                    if (st_it->second) {
                        ld.data.fulfil(*st_it->second);
                    }
                } else {
                    ld.loader = this;
                    executing = &ld;
                    ticks_left = 3;
                    return;
                }
            }
        }
    }
}
bool sim::lunit::busy() const {
    return executing != nullptr;
}
void sim::lunit::work() {
    if (!executing) return;
    ticks_left--;
}
void sim::lunit::cancel() {
    if (!executing) return;
    executing->loader = nullptr;
    executing = nullptr;
    ticks_left = -1;
}
void sim::lunit::finish() {
    if (!executing) return;
    if (ticks_left <= 0) {
        try {
            executing->data.fulfil(std::get<sim::addr_t>(sim::main_memory.at(*executing->addr)));
        } catch (const std::exception& e) {
            executing->data.error(e);
        }
        executing->loader = nullptr;
        executing = nullptr;
    }
}

void sim::sunit::start() {
    if (executing || sim::rob.empty()) return;
    if (auto st_ptr = std::get_if<store>(&sim::rob.front());
        st_ptr && st_ptr->data && st_ptr->addr && !st_ptr->storer)
        {
        st_ptr->storer = this;
        executing = st_ptr;
        ticks_left = 3;
    }
}
void sim::sunit::work() {
    if (!executing) return;
    ticks_left--;
}
bool sim::sunit::busy() const {
    return executing != nullptr;
}
void sim::sunit::cancel() {
    executing = nullptr;
    ticks_left = -1;
}
void sim::sunit::finish() {
    if (!executing) return;
    if (ticks_left <= 0) {
        // Wait until the store goes to the front of the lsq
        if (auto st_ptr = std::get_if<sim::store>(&sim::lsq.front());
            st_ptr && *st_ptr->addr == *executing->addr
                   && *st_ptr->data == *executing->data)
            {
            executing->committed = true;
            sim::lsq.pop_front();
            executing = nullptr;
        }
    }
}