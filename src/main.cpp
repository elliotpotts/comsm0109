#include <sim/control.hpp>
#include <algorithm>
#include <fmt/format.h>
#include <sim/reorder_buffer.hpp>
#include <sim/lsq.hpp>
#include <variant>
#include <random>
#include <iterator>

int main() {
    std::vector<sim::memcell> image;
    auto m = std::back_inserter(image);
    m++ = sim::ldw { 0x02, sim::areg::g0 };
    m++ = sim::halt {};
    m++ = 42;

    sim::config cfg = {
        .order = 1,
        .lsq_length = 1,
        .res_stn_count = 1,
        .rob_length = 1,
        .alu_count = 1,
        .lunit_count = 1,
        .sunit_count = 1,
        .name = "In Order"
    };
    sim::reset (cfg, image, 0);
    sim::run_until_halt();
    return 0;
}