#include <sim/insn.hpp>
#include <sim/util.hpp>
#include <sim/control.hpp>
#include <variant>
#include <algorithm>

bool sim::try_issue(const insn& any_insn) {
    return std::visit([](const auto& i) {
        return i.try_issue();
    }, any_insn);
}

sim::add::add(encoded_operand lhs, encoded_operand rhs, areg dst):
    lhs{lhs}, rhs{rhs}, dst{dst} {  
}
bool sim::add::try_issue() const {
    auto res_ptr = sim::find_reservation();
    if (res_ptr == nullptr || sim::rob.full()) return false;
    add_res res {lhs, rhs};
    sim::rat.insert_or_assign(dst, res.sum.anticipate());
    sim::rob.push_back( writeback { res.sum.anticipate(), dst });
    res_ptr->emplace(std::move(res));
    return true;
}

sim::cmp::cmp(encoded_operand lhs, encoded_operand rhs, areg dst):
    lhs{lhs}, rhs{rhs}, dst{dst} {
}
bool sim::cmp::try_issue() const {
    auto res_ptr = sim::find_reservation();
    if (res_ptr == nullptr || sim::rob.full()) return false;
    cmp_res res {lhs, rhs};
    sim::rat.insert_or_assign(dst, res.order.anticipate());
    sim::rob.push_back( writeback { res.order.anticipate(), dst});
    res_ptr->emplace(std::move(res));
    return true;
}

sim::ldw::ldw(encoded_operand address, areg dst):
    address{address}, dst{dst} {
}
bool sim::ldw::try_issue() const {
    if (sim::lsq.full() || sim::rob.full()) return false;

    sim::promise<sim::word> data;
    sim::rat.insert_or_assign(dst, data.anticipate());
    sim::lsq.push_back( load { sim::resolve_op(address), data });
    sim::rob.push_back( writeback { data.anticipate(), dst });
    return true;
}

sim::stw::stw(encoded_operand data, encoded_operand address):
    data{data}, address{address} {
}
bool sim::stw::try_issue() const {
    if (sim::lsq.full() || sim::rob.full()) return false;

    auto st = store { sim::resolve_op(data), sim::resolve_op(address) };
    sim::lsq.push_back(st);
    sim::rob.push_back(st);
    return true;
}

sim::jeq::jeq(encoded_operand lhs, encoded_operand rhs, encoded_operand offset):
    lhs{lhs}, rhs{rhs}, offset{offset} {
}
bool sim::jeq::try_issue() const {
    auto res_ptr = sim::find_reservation();
    if (res_ptr == nullptr || sim::rob.full()) return false;

    jeq_res res {lhs, rhs};
    sim::rob.push_back( branch {
        sim::resolve_op(origin + std::get<sim::word>(offset)),
        sim::ready(origin + 1),
        sim::ready(predicted), //TODO; what if there is no prediction yet?
        res.equal.anticipate()
    });
    res_ptr->emplace(std::move(res));
    return true;
}

bool sim::halt::try_issue() const {
    if (sim::rob.full()) return false;
    
    sim::rob.push_back(sim::trap {});
    return true;
}