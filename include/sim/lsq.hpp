#ifndef SIM_LSQ_HPP_INCLUDED
#define SIM_LSQ_HPP_INCLUDED

#include <sim/isa.hpp>
#include <sim/future.hpp>
#include <variant>
#include <memory>
#include <sim/util.hpp>

namespace sim {
    struct lunit;
    struct load {
        future<addr_t> addr;
        promise<word> data;
        lunit* loader = nullptr;
    };
    struct store {
        future<word> data;
        future<addr_t> addr;
        std::shared_ptr<bool> committed;
    };
    using load_store = std::variant<load, store>;
    void forward_stores();
}

#include <fmt/format.h>
namespace fmt {
    template <>
    struct formatter<sim::load_store> {
        template <typename ParseContext>
        constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

        template <typename FormatContext>
        auto format(const sim::load_store& commit, FormatContext &ctx) {
            return std::visit( match {
                [&](const sim::load& ld) {
                    return format_to(ctx.begin(), "{} ⭠ {}", ld.addr, ld.data);
                },
                [&](const sim::store& st) {
                    return format_to(ctx.begin(), "{} ⭠ {}", st.addr, st.data);
                }
            }, commit);
        }
    };
}

#endif