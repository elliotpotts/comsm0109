#ifndef SIM_UTIL_HPP_INCLUDED
#define SIM_UTIL_HPP_INCLUDED

template<class... Ts> struct match : Ts... { using Ts::operator()...; };
template<class... Ts> match(Ts...) -> match<Ts...>;

#endif