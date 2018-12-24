#include <sim/reorder_buffer.hpp>
#include <sim/util.hpp>
#include <stdexcept>

/*
bool sim::ready(sim::commitment commit) {
    return std::visit(match {
        [](writeback wb) { return wb.value.ready(); },
        [](store st) { return st.value.ready() && st.dest.ready(); },
        [](branch b) { return b.taken.ready()
                           && ((*b.taken == 1 && b.tru_offset.ready())
                           ||  (*b.taken == 0 && b.fls_offset.ready())); },
        [](halt h) { return true; },
        [](std::monostate) { return true; }
    }, commit);
}*/