#ifndef SIM_RESERVATION_STATION_HPP_INCLUDED
#define SIM_RESERVATION_STATION_HPP_INCLUDED

#include <sim/isa.hpp>
#include <sim/operand.hpp>
#include <sim/opcode.hpp>
#include <sim/future.hpp>
#include <optional>
#include <memory>

namespace sim {
    struct reservation {
        virtual bool ready() = 0;
        virtual int worktime() = 0;
        virtual void execute() = 0;
        virtual ~reservation() = default;
    };

    struct add_res : public reservation {
        future<word> lhs;
        future<word> rhs;
        promise<word> sum;
        add_res(encoded_operand, encoded_operand);
        virtual bool ready() override;
        virtual int worktime() override;
        virtual void execute() override;
    };

    std::optional<std::unique_ptr<sim::reservation>>* find_reservation();
}

#endif