#ifndef SIM_INSN_INCLUDED
#define SIM_INSN_INCLUDED

#include <sim/opcode.hpp>
#include <sim/operand.hpp>
#include <vector>

namespace sim {
    struct encoded_insn {
        sim::opcode opcode;
        std::vector<encoded_operand> operands;
        std::optional<sim::areg> destination;
    };
    struct insn {
        sim::opcode opcode;
        std::vector<sim::future<sim::word>> operands;
        bool ready();
        bool is_branch();
    };
}

#endif