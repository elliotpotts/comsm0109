#ifndef SIM_REORDER_BUFFER_HPP_INCLUDED
#define SIM_REORDER_BUFFER_HPP_INCLUDED

#include <optional>
#include <boost/circular_buffer.hpp>
#include <sim/isa.hpp>
#include <sim/future.hpp>
#include <sim/insn.hpp>

namespace sim {
    struct writeback {
        future<word> value;
        areg dest;
    };
    struct store {
        future<word> value;
        future<addr_t> dest;
    };
    struct branch {
        future<word> offset;
        future<bool> taken;
    };
    struct halt {
    };
    using commitment = std::variant<writeback, store, branch, halt>;
    struct reorder {
        live_insn instruction;
        std::optional<commitment> commit;
    };
    class reorder_buffer {
        boost::circular_buffer<reorder> commits;
    public:
        reorder_buffer(int size);
        int size();
        int capacity();
        bool full();
        void plan(live_insn, commitment);
    };
}

#endif