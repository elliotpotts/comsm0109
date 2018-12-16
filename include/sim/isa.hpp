#ifndef SIM_ISA_HPP_INCLUDED
#define SIM_ISA_HPP_INCLUDED

#include <cstdint>

namespace sim {
    enum class areg {
        g0, g1, g2, g3
    };
    using word = std::int32_t;
}

#endif