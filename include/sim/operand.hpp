#ifndef SIM_OPERAND_HPP_INCLUDED
#define SIM_OPERAND_HPP_INCLUDED

#include <variant>
#include <sim/isa.hpp>

namespace sim {
    using encoded_operand = std::variant<areg, word>;

    /*template<typename T>
    struct operand {
        std::shared_ptr<std::optional<T>> value;

        operand() : value {std::make_shared<std::optional<T>()} {
        }
            
        template<typename... S>
        T& emplace(S&&... args) {
            return value->emplace(std::forward<S>(args)...);
        }

        explicit operator bool() const {
            return value->has_value();
        }

        T& operator*() {
            return **value;
        }
        const T& operator*() const {
            return **value;
        }

        T* operator->() {
            return &**value;
        }
        const T* operator->() const {
            return &**value;
        }
    };*/
}

#endif