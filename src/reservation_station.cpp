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

std::optional<sim::reservation>* sim::find_reservation() {
    auto it = std::find(sim::res_stn.begin(), sim::res_stn.end(), std::nullopt);
    if (it == sim::res_stn.end()) {
        return nullptr;
    } else {
        return &*it;
    }
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

sim::cmp_res::cmp_res(sim::encoded_operand lhs, sim::encoded_operand rhs):
    lhs{sim::resolve_op(lhs)}, rhs{sim::resolve_op(rhs)}, order{} {
}
bool sim::cmp_res::ready() const {
    return lhs.ready() && rhs.ready();
}
int sim::cmp_res::worktime() const {
    return 1;
}
void sim::cmp_res::execute() {
    if (*lhs == *rhs) {
        order.fulfil(0);
    } else if (*lhs > *rhs) {
        order.fulfil(1);
    } else if (*lhs < *rhs) {
        order.fulfil(-1);
    }
}

sim::jeq_res::jeq_res(sim::encoded_operand lhs, sim::encoded_operand rhs):
    lhs{sim::resolve_op(lhs)}, rhs{sim::resolve_op(rhs)}, equal{} {
}
bool sim::jeq_res::ready() const {
    return lhs.ready() && rhs.ready();
}
int sim::jeq_res::worktime() const {
    return 1;
}
void sim::jeq_res::execute() {
    equal.fulfil(*lhs == *rhs);
}