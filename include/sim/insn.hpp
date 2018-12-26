#ifndef SIM_INSN_INCLUDED
#define SIM_INSN_INCLUDED

#include <sim/opcode.hpp>
#include <sim/operand.hpp>
#include <sim/isa.hpp>
#include <sim/future.hpp>
#include <sim/reservation_station.hpp>
#include <optional>
#include <vector>
#include <variant>
#include <memory>

namespace sim {
    struct insn {
        virtual bool try_issue() const = 0;
        virtual ~insn() = default;
    };

    struct add : public insn {
        encoded_operand lhs;
        encoded_operand rhs;
        areg dst;
        add(encoded_operand, encoded_operand, areg);
        virtual bool try_issue() const override;
    };

    struct ldw : public insn {
        encoded_operand address;
        areg dst;
        ldw(encoded_operand, areg);
        virtual bool try_issue() const override;
    };

    struct stw : public insn {
        encoded_operand data;
        encoded_operand address;
        stw(encoded_operand, encoded_operand);
        virtual bool try_issue() const override;
    };

    struct jeq : public insn {
        encoded_operand lhs;
        encoded_operand rhs;
        encoded_operand offset;
        jeq(encoded_operand, encoded_operand, encoded_operand);
        virtual bool try_issue() const override;
    };
    
    struct halt : public insn {
        virtual bool try_issue() const override;
    };

    struct encoded_insn {
        sim::opcode head;
        std::vector<encoded_operand> tail;
    };

    std::unique_ptr<insn> decode_at(encoded_insn, addr_t);
}

#endif