#ifndef SIM_EXECUTION_UNIT_HPP_INCLUDED
#define SIM_EXECUTION_UNIT_HPP_INCLUDED

#include <sim/isa.hpp>
#include <sim/reservation_station.hpp>
#include <sim/lsq.hpp>
#include <optional>
#include <utility>

namespace sim {
    struct execution_unit {
        virtual void start() = 0;
        virtual void work() = 0;
        virtual void finish() = 0;
        virtual ~execution_unit() = default;
    };

    class alu : public execution_unit {
        std::unique_ptr<reservation> executing;
        int ticks_left;
    public:
        virtual void start() override;
        virtual void work() override;
        virtual void finish() override;
    };

    void forward_stores();
    class lunit : public execution_unit {
        load* executing;
        int ticks_left;
    public:
        void cancel();
        virtual void start() override;
        virtual void work() override;
        virtual void finish() override;
        virtual ~lunit() = default;
    };

    class sunit : public execution_unit {
        std::optional<store> executing;
        int ticks_left;
    public:
        virtual void start() override;
        virtual void work() override;
        virtual void finish() override;
        virtual ~sunit() = default;
    };
}

#endif