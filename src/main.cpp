#include <sim/control.hpp>
#include <algorithm>
#include <fmt/format.h>
#include <sim/reorder_buffer.hpp>
#include <sim/lsq.hpp>
#include <variant>
#include <random>
#include <iterator>
using namespace sim;

int main() {
    std::vector<memcell> image = {
        1, 2
        //-79, -29, 83, 36, -20, 84, 18, -32, 89, -35, 85, -44, -6, -75, 66, -10, -46, 0,
        //3, -60, 47, 40, -11, 90, -5, 89, -33, 32, 52, 82, 85, 66, -77, -17, 97, 54, -97,
        //-35, -25, 19, -96, -70, -83, -53, -34, -70, 79, -48, -32, -10
    };
    int start = image.size();
    auto m = std::back_inserter(image);
    const areg i = areg::g0;
    const areg accum = areg::g1;
    const areg x = areg::g2;
    const areg ord = areg::g3;
    m++ = ldw {i, x};
    m++ = add {x, accum, accum};
    m++ = add {i, 1, i};
    m++ = cmp {i, start, ord};
    m++ = jeq {ord, -1, -4};
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
            sim::tick();
            sim::pdebug();
        }
    } catch (const sim::trap&) {
        fmt::print("E X E C U T I O N   H A L T E D\n");
        fmt::print("{}\n", crf[accum]);
    }
    return 0;
}