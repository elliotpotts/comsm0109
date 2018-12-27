#include <sim/reservation_station.hpp>
#include <sim/future.hpp>
#include <sim/control.hpp>
#include <algorithm>
#include <variant>

bool sim::ready(const sim::reservation& res) {
    return std::visit([](const auto& res) {
        return res.ready();
    }, res);
}
int sim::worktime(const sim::reservation& res) {
    return std::visit([](const auto& res) {
        return res.ready();
    }, res);
}
void sim::execute(sim::reservation& res) {
    std::visit([](auto& res) {
        res.execute();
    }, res);
}

sim::add_res::add_res(sim::encoded_operand lhs, sim::encoded_operand rhs):
    lhs{sim::resolve_op(lhs)},
    rhs{sim::resolve_op(rhs)},
    sum{}
    {
}
bool sim::add_res::ready() const {
    return lhs.ready() && rhs.ready();
}
int sim::add_res::worktime() const {
    return 4;
}
void sim::add_res::execute() {
    sum.fulfil(*lhs + *rhs);
}

std::optional<sim::reservation>* sim::find_reservation() {
    auto it = std::find(sim::res_stn.begin(), sim::res_stn.end(), std::nullopt);
    if (it == sim::res_stn.end()) {
        return nullptr;
    } else {
        return &*it;
    }
}