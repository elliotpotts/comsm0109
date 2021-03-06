#ifndef SIM_ISA_HPP_INCLUDED
#define SIM_ISA_HPP_INCLUDED

#include <cstdint>
#include <variant>

namespace sim {
    enum class areg {
        g0, g1, g2, g3, g4, g5, g6, g7, g8, g9
    };
    using word = std::int32_t;
    using addr_t = int;
}

#include <fmt/format.h>
namespace fmt {
    template <>
    struct formatter<sim::areg> {
        template <typename ParseContext>
        constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

        template <typename FormatContext>
        auto format(sim::areg areg, FormatContext &ctx) {
            switch (areg) {
                case sim::areg::g0: return format_to(ctx.begin(), "g0");
                case sim::areg::g1: return format_to(ctx.begin(), "g1");
                case sim::areg::g2: return format_to(ctx.begin(), "g2");
                case sim::areg::g3: return format_to(ctx.begin(), "g3");
                case sim::areg::g4: return format_to(ctx.begin(), "g4");
                case sim::areg::g5: return format_to(ctx.begin(), "g5");
                case sim::areg::g6: return format_to(ctx.begin(), "g6");
                case sim::areg::g7: return format_to(ctx.begin(), "g7");
                case sim::areg::g8: return format_to(ctx.begin(), "g8");
                case sim::areg::g9: return format_to(ctx.begin(), "g9");
                default: return format_to(ctx.begin(), "unknown register");
            }
        }
    };
}

#endif