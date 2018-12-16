#include <sim/reorder_buffer.hpp>

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