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
        .order = 6,
        .lsq_length = 20,
        .res_stn_count = 36,
        .rob_length = 40,
        .alu_count = 2,
        .lunit_count = 2,
        .sunit_count = 1,
        .name = "6-way issue out of order"
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