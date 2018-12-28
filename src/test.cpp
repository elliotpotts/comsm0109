#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <sim/control.hpp>
#include <vector>
#include <iterator>

using namespace sim;
std::vector<config> cfgs = {
    {
        .order = 1,
        .lsq_length = 1,
        .res_stn_count = 1,
        .rob_length = 1,
        .alu_count = 1,
        .lunit_count = 1,
        .sunit_count = 1,
        .name = "In Order"
    },
    {
        .order = 4,
        .lsq_length = 20,
        .res_stn_count = 36,
        .rob_length = 40,
        .alu_count = 4,
        .lunit_count = 2,
        .sunit_count = 1,
        .name = "4-way 4 alu"
    },
    {
        .order = 6,
        .lsq_length = 20,
        .res_stn_count = 36,
        .rob_length = 40,
        .alu_count = 2,
        .lunit_count = 2,
        .sunit_count = 1,
        .name = "6-way issue out of order"
    },
    {
        .order = 6,
        .lsq_length = 20,
        .res_stn_count = 36,
        .rob_length = 40,
        .alu_count = 6,
        .lunit_count = 2,
        .sunit_count = 1,
        .name = "6-way 6 alu"
    },
    {
        .order = 4,
        .lsq_length = 20,
        .res_stn_count = 36,
        .rob_length = 40,
        .alu_count = 2,
        .lunit_count = 2,
        .sunit_count = 2,
        .name = "Multiple storers"
    }
};

TEST_CASE ("Addition of immediates") {
    std::vector<memcell> image;
    auto m = std::back_inserter(image);
    m++ = add { -13, 42, areg::g0 };
    m++ = halt {};

    for (auto cfg : cfgs) {
        WHEN (cfg.name) {
            sim::reset (cfg, image, 0);
            sim::run_until_halt();
            REQUIRE ( sim::crf[areg::g0] == 42 - 13 );
        }
    }
}

TEST_CASE ("Multiplication of immediates") {
    std::vector<memcell> image;
    auto m = std::back_inserter(image);
    m++ = mul {5, 9, areg::g0};
    m++ = halt {};
    for (auto cfg : cfgs) {
        WHEN (cfg.name) {
            sim::reset(cfg, image, 0);
            sim::run_until_halt();
            REQUIRE (sim::crf[areg::g0] == 5 * 9);
        }
    }
}

TEST_CASE ("Comparison of immediates") {
    std::vector<memcell> image;
    auto m = std::back_inserter(image);
    m++ = cmp {-7, 9, areg::g0};
    m++ = cmp { 3, 3, areg::g1};
    m++ = cmp { 14, 8, areg::g2};
    m++ = halt {};
    for (auto cfg : cfgs) {
        WHEN (cfg.name) {
            sim::reset(cfg, image, 0);
            sim::run_until_halt();
            REQUIRE (sim::crf[areg::g0] == -1);
            REQUIRE (sim::crf[areg::g1] == 0);
            REQUIRE (sim::crf[areg::g2] == 1);
        }
    }
}

TEST_CASE ("Simple load") {
    std::vector<memcell> image;
    auto m = std::back_inserter(image);
    m++ = ldw { 0x02, areg::g0 };
    m++ = halt {};
    m++ = 42;
    for (auto cfg : cfgs) {
        WHEN (cfg.name) {
            sim::reset (cfg, image, 0);
            sim::run_until_halt();
            REQUIRE ( sim::crf[areg::g0] == 42);
        }
    }
}

TEST_CASE ("Simple store") {
    std::vector<memcell> image;
    auto m = std::back_inserter(image);
    m++ = stw { 42, 0x02 };
    m++ = halt {};
    m++ = 0;
    for (auto cfg : cfgs) {
        WHEN (cfg.name) {
            sim::reset (cfg, image, 0);
            sim::run_until_halt();
            REQUIRE ( std::get<word>(sim::main_memory[0x2]) == 42);
        }
    }
}

TEST_CASE ("Simple jump") {
    std::vector<memcell> image;
    auto m = std::back_inserter(image);

    m++ = jeq {areg::g0, 0, +3};
    m++ = add {50, -25, areg::g0};
    m++ = jeq {1, 1, 2};
    m++ = add {-42, 11, areg::g0};
    m++ = halt {};
    for (auto cfg : cfgs) {
        WHEN (cfg.name) {
            sim::reset (cfg, image, 0);
            sim::run_until_halt();
            REQUIRE ( sim::crf[areg::g0] == -31);
        }
    }
}

TEST_CASE ("Sum array") {
    std::vector<memcell> image = {
        -79, -29, 83, 36, -20, 84, 18, -32, 89, -35, 85, -44, -6, -75, 66, -10, -46, 0,
        3, -60, 47, 40, -11, 90, -5, 89, -33, 32, 52, 82, 85, 66, -77, -17, 97, 54, -97,
        -35, -25, 19, -96, -70, -83, -53, -34, -70, 79, -48, -32, -10
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
    for (auto cfg : cfgs) {
        WHEN (cfg.name) {
            sim::reset (cfg, image, start);
            sim::run_until_halt();
            REQUIRE ( sim::crf[accum] == 64);
        }
    }
}

TEST_CASE ("Vector sum") {
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

    for (auto cfg : cfgs) {
        WHEN (cfg.name) {
            sim::reset (cfg, image, start);
            sim::run_until_halt();
            for(unsigned i = 0; i < length; i++) {
                REQUIRE (
                    std::get<word>(main_memory[lhs_base + i]) +
                    std::get<word>(main_memory[rhs_base + i]) ==
                    std::get<word>(main_memory[res_base + i])
                );
            };
        }
    }
}

TEST_CASE ("IPC Benchmarking") {
    std::vector<memcell> image;
    auto m = std::back_inserter(image);
    for(int i = 0; i < 10000; i++) {
        m++ = add {40, 2, areg::g0};
        m++ = add {40, 2, areg::g1};
        m++ = add {40, 2, areg::g2};
        m++ = add {40, 2, areg::g3};
        m++ = add {40, 2, areg::g4};
        m++ = add {40, 2, areg::g5};
    }
    m++ = add {40, 2, areg::g0};
    m++ = add {40, 2, areg::g1};
    m++ = add {40, 2, areg::g2};
    m++ = add {40, 2, areg::g3};
    m++ = add {40, 2, areg::g4};
    m++ = halt {};
    for (auto cfg : cfgs) {
        WHEN (cfg.name) {
            sim::reset (cfg, image, 0);
            sim::run_until_halt();
            fmt::print("Clocks: {}, instructions: {}, ipc: {}\n",
                sim::cc,
                sim::ic,
                static_cast<double>(sim::ic) / sim::cc
            );
        }
    }
}

TEST_CASE ("Bubblesort") {
    std::default_random_engine rengine;
    std::uniform_int_distribution dist {-500, 500};
    std::vector<word> unsorted;
    std::generate_n(std::back_inserter(unsorted), 100, [&](){ return dist(rengine); });

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

    for (auto cfg : cfgs) {
        WHEN (cfg.name) {
            sim::reset (cfg, image, start);
            sim::run_until_halt();
            std::vector<word> sorted = unsorted;
            std::sort(sorted.begin(), sorted.end());
            std::vector<word> calculated;
            for(addr_t i = 0; i < sorted.size(); i++) {
                calculated.push_back(std::get<word>(main_memory[arr_base + i]));
            }
            REQUIRE (calculated == sorted);
            
        }
    }
}
