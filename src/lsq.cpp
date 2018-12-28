#include <sim/lsq.hpp>
#include <sim/control.hpp>
#include <algorithm>
#include <variant>
#include <boost/iterator/iterator_adaptor.hpp>

sim::load::load(sim::encoded_operand addr, promise<word> data):
    addr{sim::resolve_op(addr)}, data{data}, loader{nullptr} {
}

void sim::dismiss_loads() {
    while (!sim::lsq.empty()) {
        if (auto ld_ptr = std::get_if<sim::load>(&sim::lsq.front());
           ld_ptr && ld_ptr->data)
           {
            sim::lsq.pop_front();
        } else {
            break;
        }
    }
}