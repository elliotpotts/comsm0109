#include <sim/control.hpp>
#include <algorithm>
#include <fmt/format.h>
#include <sim/reorder_buffer.hpp>
#include <sim/lsq.hpp>
#include <variant>
#include <random>

void summarise() {
    fmt::print("------------ at t = {}: --------------------------\n", sim::t);
    fmt::print(" {:2}/{} instructions in decode buffer\n", sim::decode_buffer.size(), sim::decode_buffer.capacity());
    fmt::print(" {:2}/{} instructions in instruction queue\n", sim::insn_queue.size(), sim::insn_queue.capacity());
    fmt::print(" {:2}/{} non-empty reservation stations\n",
        std::count_if(sim::res_stn.begin(),
                      sim::res_stn.end(),
                      [](const std::optional<auto>& rs) { return rs.has_value(); }),
        sim::res_stn.size());
    fmt::print(" {:2}/{} awaiting commitments in reorder buffer\n", sim::rob.size(), sim::rob.capacity());
    for(const sim::commitment& commit : sim::rob) {
        fmt::print("    {}\n", commit);
    }
    fmt::print(" {:2}/{} items in load store queue\n", sim::lsq.size(), sim::lsq.capacity());
    for(const sim::load_store& ls : sim::lsq) {
        fmt::print("    {}\n", ls);
    }
}

int main() {
    sim::execution_units.push_back(std::make_unique<sim::alu>());
    sim::execution_units.push_back(std::make_unique<sim::alu>());
    sim::execution_units.push_back(std::make_unique<sim::alu>());
    sim::execution_units.push_back(std::make_unique<sim::lunit>());
    sim::execution_units.push_back(std::make_unique<sim::lunit>());
    sim::execution_units.push_back(std::make_unique<sim::sunit>());

    std::random_device seeder;
    std::default_random_engine engine{seeder()};
    std::uniform_int_distribution<sim::word> dist(-10, 10);
    auto& m = sim::main_memory;
    int i = 0;

    // Bubblesort program---------------------------
    int unsorted = i;
        for (int k = 0; k < 10; k++) {
            m[i++] = dist(engine);
        }
    int sorted = i;
        for (int k = 0; k < 10; k++) {
            m[i++] = -42;
        }
    int start = i;
        //m[i++] = sim::add{}
    //----------------------------------------------
    /*
    g0.k = 0;
    g1.arr_length = 10;
    g2.arr_end = arr_length - 1;
    g3.i = 0;
    g4.order = 0;
    g5.lhs = 0;
    g6.lhs_addr = 0;
    g7.rhs = 0;
    g8.rhs_addr = 0;
    g9.temp = 0;

    while (g0 < arr_length) {
        g3 <- 0;
        g9 <- 
        while (i < arr_end - g0) {
            lhs_addr <- start + i;
            lhs <- *lhs_addr;
            rhs_addr <- lhs_addr + 1;
            rhs <- *rhs_addr;
            order <- lhs <=> rhs;
            if (order == 1) {
                temp <- lhs;
                lhs  <- rhs;
                rhs  <- temp;
            }
            i++;
        }
        k++;
    }
    */
    //----------------------------------------------
    sim::pc = start;

    int max_cycles = 100;
    try {
        while (true && sim::t < max_cycles) {
            sim::tick();
            summarise();
        }
    } catch (const sim::trap& t) {
        fmt::print("\n\n\n EXECUTION HALTED\n");
        for(auto pair : sim::crf) {
            fmt::print("   {} = {}\n", pair.first, pair.second);
        }
        fmt::print("\n   {:#x} = {}\n", 0xb9, std::get<sim::word>(sim::main_memory[0xb9]));
    }
    
    return 0;
}