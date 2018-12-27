#include <sim/reorder_buffer.hpp>
#include <sim/util.hpp>
#include <sim/control.hpp>
#include <stdexcept>

sim::trap::trap() : std::runtime_error("execution halted") {
}

bool sim::ready(const sim::commitment& commit) {
    return std::visit(match {
        [](const writeback& wb) { return wb.value.ready(); },
        [](const store& st) { return false && st.data.ready() && st.addr.ready(); },
        [](const branch& b) { 
            return b.sat_offset
                && b.unsat_offset
                && b.predicted
                && b.actual; 
        },
        [](const trap&) { return true; }
    }, commit);
}

void sim::commit(const sim::commitment& commit) {
    std::visit(match {
        [](const writeback& wb) { sim::crf[wb.dest] = *wb.value; },
        [](const store& st) { sim::main_memory[*st.addr] = *st.data; },
        [](const branch& b) {
            if (*b.predicted != *b.actual) {
                if (*b.actual) {
                    // we mispredicted a branch.
                    throw trap{};
                } else {
                    // we mispredicted a branch.
                    throw trap{};
                }
            }
        },
        [](const trap& t) { throw t; }
    }, commit);
}