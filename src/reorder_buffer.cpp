#include <sim/reorder_buffer.hpp>
#include <sim/util.hpp>
#include <stdexcept>

sim::reorder_buffer::reorder_buffer(int size) : commits(size) {
}

int sim::reorder_buffer::size() {
    return commits.size();
}

int sim::reorder_buffer::capacity() {
    return commits.capacity();
}

bool sim::reorder_buffer::full() {
    return commits.full();
}

void sim::reorder_buffer::plan(sim::live_insn insn, sim::commitment commit) {
    commits.push_back({insn, commit});
}

bool sim::ready(sim::commitment commit) {
    return std::visit(match {
        [](writeback wb) { return wb.value.ready(); },
        [](store st) { return st.value.ready() && st.dest.ready(); },
        [](branch b) { return b.offset.ready() && b.taken.ready(); },
        [](halt h) { return true; },
        [](std::monostate) { return true; }
    }, commit);
}