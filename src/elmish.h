#ifndef SRC_ELMISCH_H
#define SRC_ELMISCH_H


template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

template <typename Model, typename Function, typename Variant, typename T, typename ...Ts>
inline auto update_visit_impl(
    Model& m,
    const Variant& v,
    const Function& f
) {
    if (auto x = std::get_if<T>(&v)) {
        return f(m, *x);
    } else if constexpr (sizeof...(Ts) > 0) {
        return update_visit_impl<Model, Function, Variant, Ts...>(m, v, f);
    }
}

template <typename Model, typename Function, typename ...Ts>
inline auto update_visit(
    Model& m,
    const std::variant<Ts...>& v,
    const Function& f
) {
    if (v.valueless_by_exception())
        __builtin_unreachable();

    return update_visit_impl<Model, Function, std::variant<Ts...>, Ts...>(m, v, f);
}

template<typename T>
std::string getTypeName(T&&)
{
    int status;
    char * demangled = abi::__cxa_demangle(typeid(T).name(),0,0,&status);
    std::string demangledName(demangled);
    free(demangled);
    return demangledName;
}

template<typename ...Ts>
std::string actionToString(const std::variant<Ts...>&  message)
{
    return std::visit([](auto&& arg){
        return actionToString(arg);
    }, message);
}

template<typename T>
std::string actionToString(const T& arg)
{
    return "No actionToString Type is: " + getTypeName(arg);
}

template<typename Model, typename Action>
class Store
{
public:

	const Model& model() const noexcept
	{
		return _model;
	}

	void update(const Action& action)
	{
        std::cout << actionToString(action) << "\n";
        updateModel(_model, action);
	}

private:
	Model _model;
};

#endif  //SRC_ELMISCH_H