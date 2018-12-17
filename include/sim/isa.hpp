#ifndef SIM_ISA_HPP_INCLUDED
#define SIM_ISA_HPP_INCLUDED

#include <cstdint>
#include <variant>

namespace sim {
    enum class areg {
        g0, g1, g2, g3, g4
    };
    using word = std::int32_t;
    using addr_t = int;

    /*template<typename T>
    struct located {
        int address;
        T value;
        T operator*() const {
            return value;
        }
        const T* operator->() const {
            return &value;
        }
    };*/
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
                default: return format_to(ctx.begin(), "unknown register");
            }
        }
    };
}

#endif