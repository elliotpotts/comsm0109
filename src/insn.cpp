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
    sim::rat.insert_or_assign(dst, (res.sum.anticipate()));
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
    return false;
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

    auto st = store { sim::resolve_op(data), sim::resolve_op(address), std::make_shared<bool>(false) };
    sim::lsq.push_back(st);
    sim::rob.push_back(st);
    return true;
}

sim::jeq::jeq(encoded_operand lhs, encoded_operand rhs, encoded_operand offset):
    lhs{lhs}, rhs{rhs}, offset{offset} {
}
bool sim::jeq::try_issue() const {
    if (sim::rob.full()) return false;

    /*sim::rob.push_back(branch {
        sim::resolve_op(offset), //tru_offset
        sim::ready(0), //fls_offset
        sim::resolve_op(lhs) //taken
    });*/
    return true;
}

bool sim::halt::try_issue() const {
    if (sim::rob.full()) return false;
    
    sim::rob.push_back(sim::trap {});
    return true;
}

/*
std::unique_ptr<sim::insn> sim::decode_at(sim::encoded_insn encoded, addr_t addr) {
    switch (encoded.head) {
        case opcode::add: return std::make_unique<sim::add> (
            encoded.tail[0],
            encoded.tail[1],
            std::get<areg>(encoded.tail[2])
        );
        case opcode::cmp: throw std::runtime_error("no cmp instruction yet");
        case opcode::ldw: return std::make_unique<sim::ldw> (
            encoded.tail[0],
            std::get<areg>(encoded.tail[1])
        );
        case opcode::stw: return std::make_unique<sim::stw> (
            encoded.tail[0],
            encoded.tail[1]
        );
        case opcode::jeq: return std::make_unique<sim::jeq> (
            encoded.tail[0],
            encoded.tail[1],
            encoded.tail[2]
        );
        case opcode::halt: return std::make_unique<sim::halt>();
        default: throw std::runtime_error("error decoding: unkown opcode");
    };
}*/