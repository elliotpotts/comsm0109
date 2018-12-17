#ifndef SIM_RESERVATION_STATION_HPP_INCLUDED
#define SIM_RESERVATION_STATION_HPP_INCLUDED

#include <optional>
#include <sim/insn.hpp>
#include <sim/future.hpp>
#include <vector>
#include <any>

namespace sim {
    struct reservation_station {
        live_insn waiting;
        std::vector<std::any> promises;
    };
    using reservation_station_slot = std::optional<reservation_station>;
}

#endif