
#ifndef SRC_OVERLOADED_H
#define SRC_OVERLOADED_H

template <class... Ts>
struct overloaded : Ts...
{
	using Ts::operator()...;
};
template <class... Ts>
overloaded(Ts...)->overloaded<Ts...>;

#endif  // SRC_OVERLOADED_H