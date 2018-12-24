#ifndef SIM_RESERVATION_STATION_HPP_INCLUDED
#define SIM_RESERVATION_STATION_HPP_INCLUDED

#include <sim/isa.hpp>
#include <sim/opcode.hpp>
#include <sim/future.hpp>
#include <vector>
#include <optional>

namespace sim {
    struct reservation {
        sim::opcode opcode;
        std::vector<future<word>> operands;
        promise<word> result;
        bool ready();
    };
    std::vector<std::optional<reservation>>::iterator find_reservation();
}

#endif