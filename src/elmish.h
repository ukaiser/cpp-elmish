#ifndef SRC_ELMISCH_H
#define SRC_ELMISCH_H

#include <tuple>
#include <variant>
#include <vector>
#include <functional>
#include <mutex>


namespace impl
{
    template <typename... Ts>
    using cat = decltype(std::tuple_cat(std::declval<Ts>()...));

    template <typename TResult, typename... Ts>
    struct flatten_variant;
}

namespace impl
{
    template <typename TResult>
    struct flatten_variant<TResult>
    {
        using type = TResult;
    };

    template <typename TResult, typename T, typename... TOther>
    struct flatten_variant<TResult, T, TOther...>
    {
        using type = cat<TResult, std::tuple<T>,
                         typename flatten_variant<TResult, TOther...>::type>;
    };

    template <typename TResult, typename... Ts, typename... TOther>
    struct flatten_variant<TResult, std::variant<Ts...>, TOther...>
    {
        using type =
            cat<TResult, typename flatten_variant<std::tuple<>, Ts...>::type,
                typename flatten_variant<TResult, TOther...>::type>;
    };
    
    template<typename T>
    struct to_variant;
    
    template<typename... Ts>
    struct to_variant<std::tuple<Ts...>>
    {
        using type = std::variant<Ts...>;
    };

    template<typename T>
    using flatten_variant_t = typename to_variant<typename flatten_variant<std::tuple<>, T>::type>::type;

    template<typename T>
    using flatten_variant_to_tuple_t = typename flatten_variant<std::tuple<>, T>::type;


    template<typename T, typename Tuple>
    struct TupleIndex;

    template<typename T, typename... Ts>
    struct TupleIndex<T, std::tuple<T, Ts...>>
    {
        static constexpr std::size_t value = 0;
    };

    template<typename T, typename U, typename... Ts>
    struct TupleIndex<T, std::tuple<U, Ts...>>
    {
        static constexpr std::size_t value = 1 + TupleIndex<T, std::tuple<Ts...>>::value;
    };

    //template<typename T>

}





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
    using Mutex = std::mutex;
    using Lock = std::lock_guard<Mutex>;

    using FlattenedActions = impl::flatten_variant_to_tuple_t<Action>;

    template<typename A>
    using ActionCallback = std::function<void(const A& action, const Model& model)>;
    template<typename A>
    using AugmentActionWithVectorOfCallbacks = std::vector<ActionCallback<A>>;

    template<typename T>
    struct AugmentTuple;

    template<typename... Ts>
    struct AugmentTuple<std::tuple<Ts...>>
    {
        using type = std::tuple<AugmentActionWithVectorOfCallbacks<Ts>...>;
    };

    using TupleOfVectorsWithCallbacks = typename AugmentTuple<FlattenedActions>::type;

public:


    // return by value? 
	const Model& model() const noexcept
	{
		return _model;
	}

	void update(const Action& action)
	{
        Lock lock(_mutex); // updates model only one threa at a time. Add queue???
        std::cout << actionToString(action) << "\n";
        updateModel(_model, action);
        notify(action);
	}


    template<typename A>
    void subscribe(ActionCallback<A> f)
    {
        std::get<impl::TupleIndex<A, FlattenedActions>::value>(_listener).push_back(f);
    }


private:

    template<typename ...As>
    void notify(const std::variant<As...>&  action)
    {
        return std::visit([this](auto&& arg){
            return notify(arg);
        }, action);
    }


    template<typename A>
    void notify(const A&  action)
    {
        auto&& actionListeners = std::get<impl::TupleIndex<A, FlattenedActions>::value>(_listener);
        for(auto&& listener : actionListeners)
        {
            listener(action, _model);
        } 
    }


	Model _model;
    TupleOfVectorsWithCallbacks _listener;
    Mutex _mutex;
};

#endif  //SRC_ELMISCH_H