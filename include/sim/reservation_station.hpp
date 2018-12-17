#ifndef SIM_RESERVATION_STATION_HPP_INCLUDED
#define SIM_RESERVATION_STATION_HPP_INCLUDED

#include <optional>
#include <sim/insn.hpp>
#include <sim/future.hpp>

namespace sim {
    struct reservation_station {
        live_insn waiting;
        magic_promise broadcast;
    };
    using reservation_station_slot = std::optional<reservation_station>;
}

#endif