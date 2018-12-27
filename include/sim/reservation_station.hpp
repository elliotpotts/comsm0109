#ifndef SIM_RESERVATION_STATION_HPP_INCLUDED
#define SIM_RESERVATION_STATION_HPP_INCLUDED

#include <sim/isa.hpp>
#include <sim/operand.hpp>
#include <sim/opcode.hpp>
#include <sim/future.hpp>
#include <optional>
#include <variant>

namespace sim {
    struct add_res {
        future<word> lhs;
        future<word> rhs;
        promise<word> sum;
        add_res(encoded_operand, encoded_operand);
        bool ready() const;
        int worktime() const;
        void execute();
    };

    struct mul_res {
        future<word> lhs;
        future<word> rhs;
        promise<word> product;
        mul_res(encoded_operand, encoded_operand);
        bool ready() const;
        int worktime() const;
        void execute();
    };

    struct cmp_res {
        future<word> lhs;
        future<word> rhs;
        promise<word> order;
        cmp_res(encoded_operand, encoded_operand);
        bool ready() const;
        int worktime() const;
        void execute();
    };

    struct jeq_res {
        future<word> lhs;
        future<word> rhs;
        promise<bool> equal;
        jeq_res(encoded_operand, encoded_operand);
        bool ready() const;
        int worktime() const;
        void execute();
    };

    using reservation = std::variant<add_res, mul_res, cmp_res, jeq_res>;
    bool ready(const reservation&);
    int worktime(const reservation&);
    void execute(reservation&);
    std::optional<sim::reservation>* find_reservation();
}

#endif