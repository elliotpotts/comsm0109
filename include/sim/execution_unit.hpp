#ifndef SIM_EXECUTION_UNIT_HPP_INCLUDED
#define SIM_EXECUTION_UNIT_HPP_INCLUDED

/*
#include <sim/insn.hpp>
#include <optional>
#include <utility>
#include <functional>
#include <variant>
#include <any>

namespace sim {
    class execution_unit {
        std::optional<live_insn> executing;
        std::vector<std::any> promises;
        int ticks_left;

        const std::function<bool(opcode)> handles_opcode;
        const std::function<int(live_insn)> starter;
        const std::function<void(live_insn, std::vector<std::any>)> finisher;
    public:
        template<typename HF, typename SF, typename FF>
        execution_unit(HF oc_pred, SF start, FF finish):
            handles_opcode(oc_pred), starter(start), finisher(finish)
            {
        }
        
        bool can_start(live_insn) const;
        void start(live_insn, std::vector<std::any>);
        void work();
    };
    execution_unit make_alu();
    execution_unit make_lsu();
}
*/
#endif