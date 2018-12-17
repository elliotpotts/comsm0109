#ifndef SIM_INSN_INCLUDED
#define SIM_INSN_INCLUDED

#include <sim/opcode.hpp>
#include <sim/operand.hpp>
#include <sim/isa.hpp>
#include <sim/future.hpp>
#include <optional>
#include <vector>
#include <variant>

namespace sim {
    template<typename T>
    struct insn {
        sim::opcode opcode;
        std::vector<T> operands;
        std::optional<areg> destination;
        std::optional<addr_t> writes_to;
        std::optional<addr_t> loads_from;
        std::optional<addr_t> branches_to;
    };

    using encoded_insn = insn<encoded_operand>;

    struct static_insn : public insn<encoded_operand> {
        addr_t address;
    };

    struct live_insn : public insn<future<word>> {
        addr_t address;
    };
    bool ready(live_insn);
}

namespace fmt {
    template <>
    struct formatter<sim::live_insn> {
        template <typename ParseContext>
        constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

        template <typename FormatContext>
        auto format(sim::live_insn insn, FormatContext &ctx) {
            auto it = format_to(ctx.begin(), "@{:x}({}", insn.address, insn.opcode);
            for(auto op : insn.operands) {
                it = format_to(it, " {}", op);
            }
            if (insn.destination) {
                it = format_to(it, " ⭢ {}", *insn.destination);
            }
            return format_to(it, ")");
        }
    };
}

#endif