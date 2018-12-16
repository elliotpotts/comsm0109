#ifndef SIM_REORDER_BUFFER_HPP_INCLUDED
#define SIM_REORDER_BUFFER_HPP_INCLUDED

#include <boost/circular_buffer.hpp>
#include <sim/isa.hpp>
#include <sim/future.hpp>
#include <sim/operand.hpp>

namespace sim {
    struct reg_write {
        future<sim::word> value;
        areg dest;
    };
    struct mem_write {
        future<sim::word> value;
        std::size_t dest;
    };
    struct branch {
        sim::future<sim::word> offset;
        sim::future<bool> taken;
    };
    struct halt {
    };
    using commitment = std::variant<reg_write, mem_write, branch, halt>;
    class reorder_buffer {
        boost::circular_buffer<commitment> commits;
    public:
        bool full();
        void plan(commitment);
    };
}

#endif