#ifndef SIM_LSQ_HPP_INCLUDED
#define SIM_LSQ_HPP_INCLUDED

#include <sim/isa.hpp>
#include <sim/future.hpp>
#include <variant>

namespace sim {
    struct load {
        future<addr_t> addr;
        promise<word> data;
    };
    struct store {
        future<word> data;
        future<addr_t> addr;      
    };
    using load_store = std::variant<load, store>;
}

#endif