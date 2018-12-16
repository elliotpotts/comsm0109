#include <sim/opcode.hpp>

bool sim::opcode::is_load(sim::opcode oc) {
    return oc == sim::opcode::ldw;
}