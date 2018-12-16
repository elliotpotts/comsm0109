#include <sim/reservation_station.hpp>
#include <utility>
/*
bool sim::reservation_station::empty() const {
    return !instruction.has_value();
}

sim::promise<sim::word>& sim::reservation_station::hold(sim::insn issued) {
    instruction = issued;
    result.emplace(std::in_place_type_t<sim::promise<sim::word>>{});
    return std::get<sim::promise<sim::word>>(*result);
}

sim::promise<bool>& sim::reservation_station::hold_branch(sim::insn issued) {
    instruction = issued;
    result.emplace(std::in_place_type_t<sim::promise<bool>>{});
    return std::get<sim::promise<bool>>(*result);
}*/