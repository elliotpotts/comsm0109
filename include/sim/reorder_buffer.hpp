#ifndef SIM_REORDER_BUFFER_HPP_INCLUDED
#define SIM_REORDER_BUFFER_HPP_INCLUDED

#include <optional>
#include <boost/circular_buffer.hpp>
#include <sim/isa.hpp>
#include <sim/future.hpp>
#include <sim/insn.hpp>
#include <sim/lsq.hpp>
#include <sim/util.hpp>
#include <stdexcept>
#include <string>

namespace sim {
    struct writeback {
        future<word> value;
        areg dest;
    };
    struct branch {
        future<addr_t> sat_offset;
        future<addr_t> unsat_offset;
        future<bool> predicted;
        future<bool> actual;
    };
    struct trap : public std::runtime_error {
        trap();
    };
    using commitment = std::variant<writeback, store, branch, trap>;
    bool ready(const commitment&);
    void commit(const commitment&);
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
                    return format_to (
                        ctx.begin(),
                        "branch to {} or {}. predicted {}, actual {}", 
                        br.sat_offset, br.unsat_offset,
                        br.predicted, br.actual
                    );
                },
                [&](const sim::trap& t) {
                    return format_to(ctx.begin(), "<trap>");
                }
            }, commit);
        }
    };
}

#endif