#include <sim/reorder_buffer.hpp>
#include <sim/util.hpp>
#include <sim/control.hpp>
#include <stdexcept>

sim::trap::trap() : std::runtime_error("execution halted") {
}

bool sim::ready(const sim::commitment& commit) {
    return std::visit(match {
        [](const writeback& wb) { return wb.value.ready(); },
        [](const store& st) { return st.data.ready() && st.addr.ready() && st.committed; },
        [](const branch& b) { 
            return b.sat_offset
                && b.unsat_offset
                && b.predicted
                && b.actual; 
        },
        [](const trap&) { return true; }
    }, commit);
}

bool sim::commit(const sim::commitment& commit) {
    return std::visit(match {
        [](const writeback& wb) {
            sim::crf[wb.dest] = *wb.value;
            return true;
        },
        [](const store& st) {
            sim::main_memory[*st.addr] = *st.data;
            return true;
        },
        [](const branch& b) {
            if (*b.predicted != *b.actual) {
                sim::flush();
                if (*b.actual) {
                    sim::pc = *b.sat_offset;
                } else {
                    sim::pc = *b.unsat_offset;
                }
                return false;
            } else {
                return true;
            }
        },
        [](const trap& t) { throw t; return false; }
    }, commit);
}