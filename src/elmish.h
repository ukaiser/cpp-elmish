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

    template<class T, class...Args>
    using overload_ify = decltype(
          std::declval<T>().operator()( std::declval<Args>()... )
    );

    template<class T, class Sig, class=void>
    struct has_overload:std::false_type{};

    template<class T, class R, class...Args>
    struct has_overload<T, R(Args...),
      typename std::enable_if<
        std::is_convertible<
          overload_ify<T, Args...>,
          R
        >::value
      >::type
    >:std::true_type{};

template <typename Model, typename Function, typename Variant, typename T, typename ...Ts>
inline auto update_visit_impl(
    Model& m,
    const Variant& v,
    const Function& f
) {
    if constexpr (has_overload<Function, void(Model&, T)>::value) {
        if (auto x = std::get_if<T>(&v)) {
            return f(m, *x);
        } else if constexpr (sizeof...(Ts) > 0) {
            return update_visit_impl<Model, Function, Variant, Ts...>(m, v, f);
        }
    } 
    else if constexpr (sizeof...(Ts) > 0) {
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
    using ActionModelCallback = std::function<void(const A& action, const Model& model)>;

    using ModelCallback = std::function<void(const Model& model)>;

    template<typename A>
    using ActionCallback = std::function<void(const A& action)>;
    
    template<typename A>
    using Callback = std::function<void()>;

    template<typename A>
    using AugmentActionWithVectorOfCallbacks = std::vector<ActionModelCallback<A>>;

    template<typename T>
    struct AugmentTuple;

    template<typename... Ts>
    struct AugmentTuple<std::tuple<Ts...>>
    {
        using type = std::tuple<AugmentActionWithVectorOfCallbacks<Ts>...>;
    };

    using ActionCallbacks = typename AugmentTuple<FlattenedActions>::type;
    using ModelCallbacks = std::vector<ModelCallback>;

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
        notifyModelCallbacks();

	}


    template<typename A>
    void subscribe(ActionModelCallback<A>&& f)
    {
        std::get<impl::TupleIndex<A, FlattenedActions>::value>(_actionListener)
            .emplace_back(std::forward<ActionModelCallback<A>>(f));
    }

    void subscribe(const ModelCallback& f)
    {
        _modelCallbacks.push_back(f);
    }

    void subscribe(ModelCallback&& f)
    {
        _modelCallbacks.push_back(std::move(f));
    }

    template<typename A>
    void subscribe(ActionCallback<A>&& f)
    {
        std::get<impl::TupleIndex<A, FlattenedActions>::value>(_actionListener)
            .emplace_back(
                [_f = std::forward<ActionCallback<A>>(f)](const A& action, auto&&)
                {
                    _f(action);
                }
            );
    }

    template<typename A>
    void subscribe(Callback<A>&& f)
    {
        std::get<impl::TupleIndex<A, FlattenedActions>::value>(_actionListener)
            .emplace_back(
                [_f = std::forward<Callback<A>>(f)](auto&&, auto&&)
                {
                    _f();
                }
            );
    }
private:

    void notifyModelCallbacks()
    {
        for(auto&& c : _modelCallbacks)
        {
            c(_model);
        } 
    }

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
        auto&& actionListeners = std::get<impl::TupleIndex<A, FlattenedActions>::value>(_actionListener);
        for(auto&& listener : actionListeners)
        {
            listener(action, _model);
        } 
    }


	Model _model;
    ActionCallbacks _actionListener;
    ModelCallbacks _modelCallbacks;
    Mutex _mutex;
};

#endif  //SRC_ELMISCH_H