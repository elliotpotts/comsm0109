#include <sim/reorder_buffer.hpp>
#include <sim/util.hpp>
#include <sim/control.hpp>
#include <stdexcept>

sim::trap::trap() : std::runtime_error("execution halted") {
}

bool sim::ready(sim::commitment commit) {
    return std::visit(match {
        [](const writeback& wb) { return wb.value.ready(); },
        [](const store& st) { return false && st.data.ready() && st.addr.ready(); },
        [](const branch& b) { return false; },
        [](const trap&) { return true; }
    }, commit);
}

void sim::commit(sim::commitment commit) {
    std::visit(match {
        [](const writeback& wb) { sim::crf[wb.dest] = *wb.value; },
        [](const store& st) { sim::main_memory[*st.addr] = *st.data; },
        [](const branch& b) { throw std::runtime_error("can't handle branch"); },
        [](const trap& t) { throw t; }
    }, commit);
}