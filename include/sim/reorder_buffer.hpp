#ifndef SIM_REORDER_BUFFER_HPP_INCLUDED
#define SIM_REORDER_BUFFER_HPP_INCLUDED

#include <optional>
#include <boost/circular_buffer.hpp>
#include <sim/isa.hpp>
#include <sim/future.hpp>
#include <sim/insn.hpp>
#include <sim/lsq.hpp>
#include <sim/util.hpp>

namespace sim {
    struct writeback {
        future<word> value;
        areg dest;
    };
    struct branch {
        future<addr_t> tru_offset;
        future<addr_t> fls_offset;
        future<addr_t> taken; // take branch when value != 0. todo: some better way
    };
    struct trap {
    };
    using commitment = std::variant<writeback, store, branch, trap>;
    bool ready(commitment);
}

#include <fmt/format.h>
namespace fmt {
    template <>
    struct formatter<sim::commitment> {
        template <typename ParseContext>
        constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

        template <typename FormatContext>
        auto format(const sim::commitment& commit, FormatContext &ctx) {
            return std::visit( match {
                [&](const sim::writeback& wb) {
                    return format_to(ctx.begin(), "{} ⭠ {}", wb.dest, wb.value);
                },
                [&](const sim::store& st) {
                    return format_to(ctx.begin(), "[{}] ⭠ {}", st.addr, st.data);
                },
                [&](const sim::branch& br) {
                    return format_to(ctx.begin(), "branch to +{} if {}", br.tru_offset, br.taken);
                },
                [&](const sim::trap& t) {
                    return format_to(ctx.begin(), "<trap>");
                }
            }, commit);
        }
    };
}

#endif