#include <sim/lsq.hpp>
#include <sim/control.hpp>
#include <algorithm>
#include <variant>
#include <boost/iterator/iterator_adaptor.hpp>

sim::load::load(sim::encoded_operand addr, promise<word> data):
    addr{sim::resolve_op(addr)}, data{data}, loader{nullptr} {
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
                    ld_ptr->loader = nullptr;
                }
                ld_ptr->data.fulfil(*std::get<store>(*fwdng_it).data);
            }
        }
    }
}

void sim::dismiss_loads() {
    while (!sim::lsq.empty()) {
        if (auto ld_ptr = std::get_if<sim::load>(&sim::lsq.front());
           ld_ptr && ld_ptr->data)
           {
            sim::lsq.pop_front();
        } else {
            break;
        }
    }
}