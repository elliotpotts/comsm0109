#include <sim/reorder_buffer.hpp>
#include <sim/util.hpp>
#include <stdexcept>

bool sim::ready(sim::commitment commit) {
    return std::visit(match {
        [](writeback wb) { return wb.value.ready(); },
        [](store st) { return st.value.ready() && st.dest.ready(); },
        [](branch b) { return b.offset.ready() && b.taken.ready(); },
        [](halt h) { return true; },
        [](std::monostate) { return true; }
    }, commit);
}