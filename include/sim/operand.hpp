#ifndef SIM_OPERAND_HPP_INCLUDED
#define SIM_OPERAND_HPP_INCLUDED

#include <variant>
#include <sim/isa.hpp>
#include <sim/future.hpp>

namespace sim {
    using encoded_operand = std::variant<areg, sim::word>;
}

#endif