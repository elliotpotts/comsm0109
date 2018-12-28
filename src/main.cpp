#include <sim/control.hpp>
#include <algorithm>
#include <fmt/format.h>
#include <sim/reorder_buffer.hpp>
#include <sim/lsq.hpp>
#include <variant>
#include <random>
#include <iterator>
#include <iostream>
using namespace sim;

int main() {
    std::default_random_engine rengine;
    std::uniform_int_distribution dist {-500, 500};
    std::vector<word> unsorted;
    std::generate_n(std::back_inserter(unsorted), 1000, [&](){ return dist(rengine); });

    std::vector<memcell> image;
    int arr_base = image.size();
    std::copy(unsorted.begin(), unsorted.end(), std::back_inserter(image));
    int last_offset = unsorted.size() - 1;

    int start = image.size();
    const areg i = areg::g0;
    const areg swaps = areg::g1;
    const areg ord = areg::g2;
    const areg lhs_addr = areg::g3;
    const areg lhs = areg::g4;
    const areg rhs_addr = areg::g5;
    const areg rhs = areg::g6;
    auto m = std::back_inserter(image);

    m++ = add {0, 0, swaps};
    m++ = add {0, 0, i};
        m++ = add {arr_base, i, lhs_addr};
        m++ = ldw {lhs_addr, lhs};
        m++ = add {lhs_addr, 1, rhs_addr};
        m++ = ldw {rhs_addr, rhs};
        m++ = cmp {lhs, rhs, ord};
        m++ = jeq {ord, 0, 5};
        m++ = jeq {ord, -1, 4};
            m++ = stw {lhs, rhs_addr};
            m++ = stw {rhs, lhs_addr};
            m++ = add {swaps, 1, swaps};
        m++ = add {i, 1, i};
        m++ = cmp {i, last_offset, ord};
    m++ = jeq {ord, -1, -12};
    m++ = cmp {swaps, 0, ord};
    m++ = jeq {ord, 1, -16};
    m++ = halt {};

    sim::config cfg = {
        .order = 4,
        .lsq_length = 20,
        .res_stn_count = 36,
        .rob_length = 40,
        .alu_count = 4,
        .lunit_count = 2,
        .sunit_count = 1,
        .name = ""
    };
    sim::reset (cfg, image, start);
    try {
        while (true) {
            std::string dummy;
            std::getline(std::cin, dummy);
            sim::tick();
            sim::pdebug();
        }
    } catch (const sim::trap&) {
        fmt::print("E X E C U T I O N   H A L T E D\n");
        sim::print_stats();
    }
    return 0;
}