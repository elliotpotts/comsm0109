#ifndef SIM_RESERVATION_STATION_HPP_INCLUDED
#define SIM_RESERVATION_STATION_HPP_INCLUDED

#include <optional>
#include <sim/insn.hpp>
#include <sim/future.hpp>
#include <sim/operand.hpp>

namespace sim {
    class execution_unit;
    class reservation_station {
        std::optional<insn> instruction;
        std::optional<std::variant<sim::promise<sim::word>, sim::promise<bool>>> result;
        //std::optional<sim::promise<std::variant<sim::word, bool>>> result;
        execution_unit* executor;
    public:
        bool empty() const;
        sim::promise<sim::word>& hold(insn);
        sim::promise<bool>& hold_branch(insn);
    };
}

#endif