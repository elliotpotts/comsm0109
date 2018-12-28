#ifndef SIM_FUTURE_HPP_INCLUDED
#define SIM_FUTURE_HPP_INCLUDED

#include <stdexcept>
#include <optional>
#include <memory>
#include <fmt/format.h>

namespace sim {
    inline unsigned fpid = 0;

    template<typename T>
    class promise;

    template<typename T>
    class future;

    template<typename T>
    future<T> ready(T value) {
        return {fpid++, std::make_shared<std::optional<T>>(value), {}};
    }

    template<typename T>
    future<T> never() {
        return {fpid++, std::make_shared<std::optional<T>>(std::nullopt), {}};
    }

    template<typename T>
    class future {
        std::shared_ptr<std::optional<T>> result;
        future(unsigned id
              ,std::shared_ptr<std::optional<T>> result
              ,std::shared_ptr<std::exception> exception):
              result{result}, exception{exception}, id{id} {
        }
    public:
        std::shared_ptr<std::exception> exception;
        unsigned id;
        friend future<T> promise<T>::anticipate() const;
        friend future<T> ready<>(T);
        friend future<T> never<T>();
        bool ready() const {
            return result->has_value() || exception;
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
        std::shared_ptr<std::exception> exception;
        unsigned id;
        promise() : result(std::make_shared<std::optional<T>>()), id(fpid++) {
        }
        void fulfil(T value) {
            if (*this) {
                throw std::runtime_error("Fulfilled promise cannot be fulfilled again.");
            } else {
                *result = value;
            }
        }
        void error(const std::exception& e) {
            exception = std::make_shared<std::exception>(e);
        }
        future<T> anticipate() const {
            return future<T>{id, result, exception};
        }

        explicit operator bool() const {
            return result->has_value() || exception;
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
            std::string err_marker = static_cast<bool>(f.exception) ? "!" : "";
            if (f) {
                return format_to(ctx.begin(), "{}{}/{}", err_marker, *f, f.id);
            } else {
                return format_to(ctx.begin(), "⧗{}{}", err_marker, f.id);
            }
        }
    };
    template <typename T>
    struct formatter<sim::promise<T>> {
        template <typename ParseContext>
        constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

        template <typename FormatContext>
        auto format(const sim::promise<T> &p, FormatContext &ctx) {
            std::string err_marker = static_cast<bool>(p.exception) ? "!" : "";
            if (p) {
                return format_to(ctx.begin(), "{}{}/{}", err_marker, *p, p.id);
            } else {
                return format_to(ctx.begin(), "π{}{}", err_marker, p.id);
            }
        }
    };
}

#endif