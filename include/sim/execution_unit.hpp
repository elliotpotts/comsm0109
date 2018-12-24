#ifndef SIM_EXECUTION_UNIT_HPP_INCLUDED
#define SIM_EXECUTION_UNIT_HPP_INCLUDED

#include <sim/isa.hpp>
#include <sim/reservation_station.hpp>
#include <sim/opcode.hpp>
#include <optional>
#include <utility>
#include <functional>

namespace sim {
    class execution_unit {
        std::optional<opcode> oc;
        std::vector<word> operands;
        std::optional<promise<word>> computed;
        int ticks_left;

        const std::function<bool(opcode)> mask;
        const std::function<int(opcode, std::vector<word>)> time;
        const std::function<void(opcode, std::vector<word>, promise<word>)> execute;
    public:
        template<typename S, typename T, typename U>
        execution_unit(S mask, T time, U execute):
            mask(mask), time(time), execute(execute)
            {
        }
        
        bool can_start(const reservation&) const;
        void start(const reservation&);
        void work();
    };
    execution_unit make_alu();
    //execution_unit make_lsu();
}

#endif