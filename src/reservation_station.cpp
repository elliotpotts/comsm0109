#include <sim/reservation_station.hpp>
#include <sim/future.hpp>
#include <sim/control.hpp>
#include <algorithm>

sim::add_res::add_res(sim::encoded_operand lhs, sim::encoded_operand rhs):
    lhs{sim::resolve_op(lhs)},
    rhs{sim::resolve_op(rhs)},
    sum{}
    {
}
bool sim::add_res::ready() {
    return lhs.ready() && rhs.ready();
}
int sim::add_res::worktime() {
    return 4;
}
void sim::add_res::execute() {
    sum.fulfil(*lhs + *rhs);
}

std::optional<std::unique_ptr<sim::reservation>>* sim::find_reservation() {
    auto it = std::find_if(sim::res_stn.begin(),
                           sim::res_stn.end(),
                           [](const std::optional<std::unique_ptr<sim::reservation>>& rs) {
                               return !static_cast<bool>(rs);
                           });
    if (it == sim::res_stn.end()) {
        return nullptr;
    } else {
        return &*it;
    }
}