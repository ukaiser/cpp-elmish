
#include <variant>
#include <vector>
#include <iostream>
#include <string>
#include <cxxabi.h>

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

/*
template<typename T>
std::string messageToString(const T& t)
{
    return getTypeName(t);
}
*/

struct IncrementBy
{
	int value;
};

struct DecrementBy
{
	int value;
};

struct CountModel
{
	int counter = 0;
};

#if 0
auto updateIncrementBy = [](CountModel &model, const IncrementBy &incrementBy) {
  model.counter += incrementBy.value;
};
#else
struct UpdateIncrementBy
{
    void operator()(CountModel &model, const IncrementBy &incrementBy) const {
        model.counter += incrementBy.value;
    }
};
auto updateIncrementBy = UpdateIncrementBy{};
#endif

std::string messageToString(const IncrementBy&  incrementBy)
{
    return "IncrementBy";
}


auto updateDecrementBy = [](CountModel &model, const DecrementBy &decrementtBy) {
  model.counter -= decrementtBy.value;
};

std::string messageToString(const DecrementBy&  decrementBy)
{
    return "DecrementBy";
}

using CountMessage = std::variant<IncrementBy, DecrementBy>;



struct CameraConnected {};
struct CameraDisconnected {};

using CameraMessage = std::variant<CameraConnected, CameraDisconnected>;
struct CameraConnectedModel {
  bool isConnected = false;
};

auto updateCameraConnected = [](CameraConnectedModel &model,
                      const CameraConnected &) {
  model.isConnected += true;
};

std::string messageToString(const CameraConnected&  cameraConnected)
{
    return "cameraConnected";
}


auto updateCameraDisconnected = [](CameraConnectedModel &model, const CameraDisconnected &) {
  model.isConnected -= false;
};

std::string messageToString(const CameraDisconnected&  cameraDisconnected)
{
    return "cameraDisconnected";
}


struct AppModel {
  CountModel countModel;
  CameraConnectedModel cameraConnectedModel;
};

auto updateCountModel = [](AppModel &model, const CountMessage &countMessage) {
        update_visit(model.countModel, countMessage, overloaded {
            updateIncrementBy,
            updateDecrementBy
        });
};

std::string messageToString(const CountMessage&  countMessage)
{
    return std::visit([](auto&& arg){
        return messageToString(arg);
    }, countMessage);
};

auto updateCameraModle = [](AppModel &model, const CameraMessage &cameraMessage) {
        update_visit(model.cameraConnectedModel, cameraMessage, overloaded {
            updateCameraConnected,
            updateCameraDisconnected
        });
};

using AppMessage = std::variant<CountMessage, CameraMessage>;
void updateModel(AppModel &model, const AppMessage& message)
{
    update_visit(model, message, overloaded {
        updateCountModel,
        updateCameraModle
    });   
}   


template<typename ...Ts>
std::string messageToString(const std::variant<Ts...>&  message)
{
    return std::visit([](auto&& arg){
        return messageToString(arg);
    }, message);
};


template<typename Model, typename Message>
class Store
{
public:
	const Model& model() const noexcept
	{
		return _model;
	}

	void update(const Message& message)
	{
        std::cout << messageToString(message) << "\n";
        updateModel(_model, message);
	}

private:
	Model _model;
};

using AppStore = Store<AppModel, AppMessage>;


int main()
{
  AppStore appStore;
	
  appStore.update(IncrementBy{2});
  std::cout << "counter: " << appStore.model().countModel.counter << '\n';

  appStore.update(IncrementBy{3});
  std::cout << "counter: " << appStore.model().countModel.counter << '\n';

  std::cout << "isConnected: "
            << appStore.model().cameraConnectedModel.isConnected << '\n';

  appStore.update(CameraConnected{});

  std::cout << "isConnected: "
            << appStore.model().cameraConnectedModel.isConnected << '\n';

  appStore.update(DecrementBy{4});
  std::cout << "counter: " << appStore.model().countModel.counter << '\n';
}


