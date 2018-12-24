#ifndef SIM_REORDER_BUFFER_HPP_INCLUDED
#define SIM_REORDER_BUFFER_HPP_INCLUDED

#include <optional>
#include <boost/circular_buffer.hpp>
#include <sim/isa.hpp>
#include <sim/future.hpp>
#include <sim/insn.hpp>
#include <sim/lsq.hpp>

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

#endif