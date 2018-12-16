#ifndef SIM_EXECUTION_UNIT_HPP_INCLUDED
#define SIM_EXECUTION_UNIT_HPP_INCLUDED

#include <function>

namespace sim {
    class reservation_station;
    class execution_unit {
        reservation_station* to_clear;
    };
}

#endif