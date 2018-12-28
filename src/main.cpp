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
    std::uniform_int_distribution dist {-10, 10};
    int length = 3;

    std::vector<memcell> image;
    int lhs_base = image.size();
    std::generate_n(std::back_inserter(image), length, [&](){ return dist(rengine); });
    int rhs_base = image.size();
    std::generate_n(std::back_inserter(image), length, [&](){ return dist(rengine); });
    int res_base = image.size();
    image.insert(image.end(), length, 0);

    int start = image.size();
    const areg offset = areg::g0;
    const areg addr = areg::g1;
    const areg lhs = areg::g2;
    const areg rhs = areg::g3;
    const areg sum = areg::g4;
    const areg ord = areg::g5;
    auto m = std::back_inserter(image);
    m++ = add {offset, lhs_base, addr};
    m++ = ldw {addr, lhs};
    m++ = add {offset, rhs_base, addr};
    m++ = ldw {addr, rhs};
    m++ = add {lhs, rhs, sum};
    m++ = add {offset, res_base, addr};
    m++ = stw {sum, addr};
    m++ = add {offset, 1, offset};
    m++ = cmp {offset, length, ord};
    m++ = jeq {ord, -1, -9};
    m++ = halt {};

    sim::config cfg = {
        .order = 6,
        .lsq_length = 20,
        .res_stn_count = 36,
        .rob_length = 40,
        .alu_count = 2,
        .lunit_count = 2,
        .sunit_count = 1,
        .name = "6-way issue out of order"
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
        for(int i = 0; i < length; i++) {
            fmt::print("{} + {} = {}\n",
                std::get<sim::word>(sim::main_memory[lhs_base + i]),
                std::get<sim::word>(sim::main_memory[rhs_base + i]),
                std::get<sim::word>(sim::main_memory[res_base + i])
            );
        }
    }
    return 0;
}