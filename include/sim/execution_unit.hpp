#ifndef SIM_EXECUTION_UNIT_HPP_INCLUDED
#define SIM_EXECUTION_UNIT_HPP_INCLUDED

#include <sim/isa.hpp>
#include <sim/reservation_station.hpp>
#include <sim/opcode.hpp>
#include <optional>
#include <utility>
#include <functional>

namespace sim {
    class execution_unit {
        std::unique_ptr<reservation> executing;
        int ticks_left;
    public:
        void dispatch();
        void work();
        void broadcast();
    };
}

#endif