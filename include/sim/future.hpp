#ifndef SIM_FUTURE_HPP_INCLUDED
#define SIM_FUTURE_HPP_INCLUDED

#include <stdexcept>
#include <optional>
#include <memory>

namespace sim {
    template<typename T>
    class promise;

    template<typename T>
    class future;

    template<typename T>
    future<T> immediate(T value) {
        return {std::make_shared<std::optional<T>>(value)};
    }

    template<typename T>
    class future {
        std::shared_ptr<std::optional<T>> result;
        future(std::shared_ptr<std::optional<T>> result) : result(result) {
        }
    public:
        friend future<T> promise<T>::anticipate();
        friend future<T> immediate<>(T);
        explicit operator bool() const noexcept {
            return result->has_value();
        }
        T operator*() const {
            if(*this) {
                return **result;
            } else {
                throw std::runtime_error("Future prematurely accessed");
            }
        }
    };

    template<typename T>
    class promise {
        friend class future<T>;
        std::shared_ptr<std::optional<T>> result;
    public:
        promise() : result(std::make_shared<std::optional<T>>()) {
        }
        void fulfil(T value) {
            *result = value;
        }
        future<T> anticipate() {
            return future<T>{result};
        }
    };
}

#endif