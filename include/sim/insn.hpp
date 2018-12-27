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
    struct add {
        encoded_operand lhs;
        encoded_operand rhs;
        areg dst;
        add(encoded_operand, encoded_operand, areg);
        bool try_issue() const;
    };

    struct mul {
        encoded_operand lhs;
        encoded_operand rhs;
        areg dst;
        mul(encoded_operand, encoded_operand, areg);
        bool try_issue() const;
    };

    struct cmp {
        encoded_operand lhs;
        encoded_operand rhs;
        areg dst;
        cmp(encoded_operand, encoded_operand, areg);
        bool try_issue() const;
    };

    struct ldw {
        encoded_operand address;
        areg dst;
        ldw(encoded_operand, areg);
        bool try_issue() const;
    };

    struct stw {
        encoded_operand data;
        encoded_operand address;
        stw(encoded_operand, encoded_operand);
        bool try_issue() const;
    };

    struct jeq  {
        encoded_operand lhs;
        encoded_operand rhs;
        encoded_operand offset;
        bool predicted = false;
        addr_t origin = 0xdeadbeef;
        jeq(encoded_operand, encoded_operand, encoded_operand);
        bool try_issue() const;
    };
    
    struct halt {
        bool try_issue() const;
    };

    using insn = std::variant<add, mul, cmp, ldw, stw, jeq, halt>;
    bool try_issue(const insn&);

    using encoded_insn = insn;
}

#endif