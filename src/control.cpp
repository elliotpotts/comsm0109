#include <sim/control.hpp>
#include <sim/operand.hpp>
#include <sim/util.hpp>
#include <sim/future.hpp>
#include <fmt/format.h>
#include <sim/insn.hpp>
#include <algorithm>
#include <iterator>
#include <variant>

int sim::branches = 0;
int sim::mispredicts = 0;
int sim::cc = 0;
int sim::ic = 0;
sim::addr_t sim::pc = 0;
std::vector<sim::memcell> sim::main_memory;
boost::circular_buffer<sim::encoded_insn> sim::decode_buffer;
boost::circular_buffer<sim::insn> sim::insn_queue;
std::unordered_map<sim::areg, sim::future<sim::word>> sim::rat;
boost::circular_buffer<sim::load_store> sim::lsq;
std::vector<std::optional<sim::reservation>> sim::res_stn;
std::vector<std::unique_ptr<sim::execution_unit>> sim::execution_units;
boost::circular_buffer<sim::commitment> sim::rob;
std::map<sim::areg, sim::word> sim::crf;

void sim::reset(sim::config cfg, const std::vector<sim::memcell>& image, addr_t start) {
    sim::branches = 0;
    sim::mispredicts = 0;
    sim::ic = 0;
    sim::cc = 0;
    sim::pc = start;
    sim::main_memory = image;
    sim::decode_buffer.clear();
    sim::decode_buffer.set_capacity(cfg.order);
    sim::insn_queue.clear();
    sim::insn_queue.set_capacity(cfg.order);
    sim::rat.clear();
    sim::lsq.clear();
    sim::lsq.set_capacity(cfg.lsq_length);
    sim::res_stn.clear();
    sim::res_stn.resize(cfg.res_stn_count);
    sim::execution_units.clear();
    for(int i = 0; i < cfg.alu_count; i++)
        sim::execution_units.push_back(std::make_unique<sim::alu>());
    for(int i = 0; i < cfg.lunit_count; i++)
        sim::execution_units.push_back(std::make_unique<sim::lunit>());
    for(int i = 0; i < cfg.sunit_count; i++)
        sim::execution_units.push_back(std::make_unique<sim::sunit>());
    sim::rob = boost::circular_buffer<sim::commitment>{ static_cast<unsigned>(cfg.rob_length) };
    sim::crf.clear();
}

void sim::run_until_halt() {
    try {
        while (true) {
            sim::tick();
        }
    } catch (const sim::trap&) {
    }
}

void sim::print_stats() {
    fmt::print("   Clock count: {}\n", sim::cc);
    fmt::print("    Insn count: {}\n", sim::ic);
    fmt::print("           IPC: {}\n", static_cast<double>(sim::ic) / sim::cc);
    if (sim::branches > 0) {
        fmt::print("Branch hitrate: {:2}%\n",
            static_cast<double>(sim::branches - sim::mispredicts)/sim::branches * 100);
    } else {
        fmt::print("Branch hitrate: (no branches)\n");
    }
}

void sim::pdebug() {
    fmt::print("------------ at t = {}: --------------------------\n", sim::cc);
    fmt::print(" {:2}/{} instructions in decode buffer\n", sim::decode_buffer.size(), sim::decode_buffer.capacity());
    fmt::print(" {:2}/{} instructions in instruction queue\n", sim::insn_queue.size(), sim::insn_queue.capacity());
    fmt::print(" {:2}/{} non-empty reservation stations\n",
        std::count_if(sim::res_stn.begin(),
                      sim::res_stn.end(),
                      [](const std::optional<auto>& rs) { return rs.has_value(); }),
        sim::res_stn.size());
    for(const std::optional<sim::reservation>& res : sim::res_stn) {
        if (res) {
            fmt::print("    {}\n", *res);
        }
    }
    fmt::print(" {:2}/{} execution units busy\n",
        std::count_if (
            execution_units.begin(),
            execution_units.end(),
            [](const std::unique_ptr<execution_unit>& eu) {
                return eu->busy();
            }
        ),
        execution_units.size()
    );
    fmt::print(" {:2}/{} awaiting commitments in reorder buffer\n", sim::rob.size(), sim::rob.capacity());
    for(const sim::commitment& commit : sim::rob) {
        fmt::print("    {}\n", commit);
    }
    fmt::print(" {:2}/{} items in load store queue\n", sim::lsq.size(), sim::lsq.capacity());
    for(const sim::load_store& ls : sim::lsq) {
        fmt::print("    {}\n", ls);
    }
}

sim::future<sim::word> sim::resolve_op(encoded_operand operand) {
    return std::visit(match {
        [](sim::word immediate) { return sim::ready(immediate); },
        [](sim::areg from_reg) {
            if (auto fut_it = rat.find(from_reg); fut_it == rat.end()) {
                return sim::ready(crf[from_reg]);
            } else {
                return fut_it->second;
            }
        }
    }, operand);
}

void sim::flush() {
    sim::mispredicts++;
    sim::decode_buffer.clear();
    sim::insn_queue.clear();
    sim::rat.clear();
    sim::lsq.clear();
    for(auto& stn : sim::res_stn) {
        stn.reset();
    }
    for(auto& eu : sim::execution_units) {
        eu->cancel();
    }
    sim::rob.clear();
}

void sim::fetch() {
    if (true) {
        while (!decode_buffer.full()
            && sim::pc < static_cast<sim::addr_t>(sim::main_memory.size())) {
            bool stop_fetching = false;
            std::visit ( match {
                [&](sim::word data) {
                    stop_fetching = true; // stop fetching
                },
                [&](sim::encoded_insn& encoded) {
                    // pre-decode to see if we hit a branch
                    if (auto br_ptr = std::get_if<sim::jeq>(&encoded); br_ptr) {
                        br_ptr->origin = sim::pc;
                        return std::visit( match {
                            [&](const sim::word offset) {
                                // TODO: improve branch prediction
                                if (offset < 0) {
                                    sim::pc += offset;
                                    br_ptr->predicted = true;
                                } else {
                                    sim::pc++;
                                    br_ptr->predicted = false;
                                }
                                sim::decode_buffer.push_back(encoded);
                            },
                            [&](const sim::areg reg) {
                                throw std::runtime_error("can't deal with reg dst");
                                stop_fetching = true; // stall until we have the address
                            }
                        }, br_ptr->offset);
                    } else {
                        sim::pc = sim::pc + 1;
                        sim::decode_buffer.push_back(encoded);
                    }
                }
            }, sim::main_memory[sim::pc]);
            if (stop_fetching) {
                break;
            };
        }
    }
}

void sim::decode() {
    while (!decode_buffer.empty() && !insn_queue.full()) {
        sim::insn decoded = std::move(decode_buffer.front());
        insn_queue.push_back(std::move(decoded));
        decode_buffer.pop_front();
    }
}

void sim::issue() {
    while(!insn_queue.empty() && !rob.full()) {
        if (sim::try_issue(insn_queue.front())) {
            insn_queue.pop_front();
        } else {
            break;
        }
    }
}

void sim::execute() {
    dismiss_loads();
    for (auto& eu : sim::execution_units) {
        eu->start();
    }
    for (auto& eu : sim::execution_units) {
        eu->work();
    }
    for (auto& eu : sim::execution_units) {
        eu->finish();
    }
}

void sim::commit() {
    int commits_left = sim::insn_queue.capacity();
    while (!rob.empty() && commits_left > 0) {
        if (sim::ready(sim::rob.front())) {
            commits_left--;
            if(sim::commit(sim::rob.front())) {
                break;
            } else {
                sim::ic++;
                sim::rob.pop_front();
            }
        } else {
            break;
        }
    }
}

void sim::tick() {
    sim::commit();
    sim::execute();
    sim::issue();
    sim::decode();
    sim::fetch();
    sim::cc++;
}