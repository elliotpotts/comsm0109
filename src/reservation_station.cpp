#include <sim/reservation_station.hpp>
#include <sim/future.hpp>
#include <algorithm>

bool sim::reservation::ready() {
    return std::all_of (
        operands.begin(),
        operands.end(),
        [](const sim::future<word>& op) { return op.ready(); }
    );
}