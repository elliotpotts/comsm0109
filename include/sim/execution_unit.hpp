#ifndef SIM_EXECUTION_UNIT_HPP_INCLUDED
#define SIM_EXECUTION_UNIT_HPP_INCLUDED

#include <sim/isa.hpp>
#include <sim/reservation_station.hpp>
#include <sim/lsq.hpp>
#include <optional>
#include <utility>

namespace sim {
    struct execution_unit {
        virtual void cancel() = 0;
        virtual void start() = 0;
        virtual void work() = 0;
        virtual void finish() = 0;
        virtual ~execution_unit() = default;
    };

    class alu : public execution_unit {
        std::optional<reservation> executing;
        int ticks_left;
    public:
        virtual void cancel() override;
        virtual void start() override;
        virtual void work() override;
        virtual void finish() override;
    };

    class lunit : public execution_unit {
        load* executing;
        int ticks_left;
    public:
        virtual void cancel() override;
        virtual void start() override;
        virtual void work() override;
        virtual void finish() override;
        virtual ~lunit() = default;
    };

    class sunit : public execution_unit {
        store* executing;
        int ticks_left;
    public:
        virtual void cancel() override;
        virtual void start() override;
        virtual void work() override;
        virtual void finish() override;
        virtual ~sunit() = default;
    };
}

#endif