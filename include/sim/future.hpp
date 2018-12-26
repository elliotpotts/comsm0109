#ifndef SIM_FUTURE_HPP_INCLUDED
#define SIM_FUTURE_HPP_INCLUDED

#include <stdexcept>
#include <optional>
#include <memory>
#include <fmt/format.h>

namespace sim {
    template<typename T>
    class promise;

    template<typename T>
    class future;

    template<typename T>
    future<T> ready(T value) {
        return {std::make_shared<std::optional<T>>(value)};
    }

    template<typename T>
    future<T> never() {
        return {std::make_shared<std::optional<T>>(std::nullopt)};
    }

    template<typename T>
    class future {
        std::shared_ptr<std::optional<T>> result;
        future(std::shared_ptr<std::optional<T>> result) : result(result) {
        }
    public:
        friend future<T> promise<T>::anticipate();
        friend future<T> ready<>(T);
        friend future<T> never<T>();
        bool ready() const {
            return result->has_value();
        }
        explicit operator bool() const {
            return ready();
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
            if (*this) {
                throw std::runtime_error("Fulfilled promise cannot be fulfilled again.");
            } else {
                *result = value;
            }
        }
        future<T> anticipate() {
            return future<T>{result};
        }

        explicit operator bool() const {
            return result->has_value();
        }
        const T& operator*() const {
            return **result;
        }
        T& operator*() {
            return **result;
        }
    };
}

namespace fmt {
    template <typename T>
    struct formatter<sim::future<T>> {
        template <typename ParseContext>
        constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

        template <typename FormatContext>
        auto format(const sim::future<T> &f, FormatContext &ctx) {
            if (f) {
                return format_to(ctx.begin(), "{}", *f);
            } else {
                return format_to(ctx.begin(), "⧗");
            }
        }
    };
    template <typename T>
    struct formatter<sim::promise<T>> {
        template <typename ParseContext>
        constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

        template <typename FormatContext>
        auto format(const sim::promise<T> &p, FormatContext &ctx) {
            if (p) {
                return format_to(ctx.begin(), "{}", *p);
            } else {
                return format_to(ctx.begin(), "□");
            }
        }
    };
}

#endif