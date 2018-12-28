#include <sim/control.hpp>
#include <vector>
#include <random>

using namespace sim;

void xp1() {
    std::vector<memcell> image;
    int start = image.size();
    auto m = std::back_inserter(image);
    m++ = add {areg::g0, 1, areg::g0};
    m++ = add {areg::g1, 1, areg::g1};
    m++ = add {areg::g2, 1, areg::g2};
    m++ = add {areg::g3, 1, areg::g3};
    m++ = cmp {areg::g0, 50000, areg::g4};
    m++ = jeq {areg::g4, -1, -5};
    m++ = halt {};

    std::vector<config> cfgs;
    for(int i = 1; i < 11; i++) {
        cfgs.push_back(config {
            .order = 6,
            .lsq_length = 20,
            .res_stn_count = 36,
            .rob_length = 40,
            .alu_count = i,
            .lunit_count = 2,
            .sunit_count = 1,
            .name = ""
        });
    }

    for (auto cfg : cfgs) {
        fmt::print("xp1/{} alus\n", cfg.alu_count);
        sim::reset (cfg, image, start);
        sim::run_until_halt();
        sim::print_stats();
    }
}

void xp2() {
    std::default_random_engine rengine;
    std::uniform_int_distribution dist {-500, 500};
    std::vector<word> unsorted;
    std::generate_n(std::back_inserter(unsorted), 200, [&](){ return dist(rengine); });

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

    std::vector<config> cfgs;
    for(int i = 1; i < 7; i++) {
        cfgs.push_back(config {
            .order = i,
            .lsq_length = 20,
            .res_stn_count = 36,
            .rob_length = 40,
            .alu_count = 2,
            .lunit_count = 2,
            .sunit_count = 1,
            .name = ""
        });
    }

    for (auto cfg : cfgs) {
        fmt::print("xp2/{}-way out of order\n", cfg.order);
        sim::reset (cfg, image, start);
        sim::run_until_halt();
        sim::print_stats();
    }
}

int main() {
    //xp1();
    xp2();
    return 0;
}