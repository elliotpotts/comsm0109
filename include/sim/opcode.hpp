#ifndef SIM_OPCODE_HPP_INCLUDED
#define SIM_OPCODE_HPP_INCLUDED

namespace sim {
    enum class opcode {
        add,
        cmp,
        ldw,
        stw,
        jnz,
        halt
    };
    //bool is_load(opcode oc);
}

#include <string_view>
#include <fmt/format.h>
template <>
struct fmt::formatter<sim::opcode> : formatter<std::string_view> {
    template <typename FormatContext>
    auto format(sim::opcode oc, FormatContext &ctx) {
        std::string_view name = "unknown";
        switch (oc) {
            case sim::opcode::add:  name = "add";  break;
            case sim::opcode::cmp:  name = "cmp";  break;
            case sim::opcode::ldw:  name = "ldw";  break;
            case sim::opcode::stw:  name = "stw";  break;
            case sim::opcode::jnz:  name = "jnz";  break;
            case sim::opcode::halt: name = "halt"; break;
        }
        return formatter<std::string_view>::format(name, ctx);
    }
};

#endif