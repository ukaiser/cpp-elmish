
#include <variant>
#include <vector>
#include <iostream>


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

auto updateIncrementBy = [](CountModel &model, const IncrementBy &incrementBy) {
  model.counter += incrementBy.value;
};

auto updateDecrementBy = [](CountModel &model, const DecrementBy &decrementtBy) {
  model.counter -= decrementtBy.value;
};


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

auto updateCameraDisconnected = [](CameraConnectedModel &model, const CameraDisconnected &) {
  model.isConnected -= false;
};

struct AppModel {
  CountModel countModel;
  CameraConnectedModel cameraConnectedModel;
};

using AppMessage = std::variant<CountMessage, CameraMessage>;

auto updateCountModel = [](AppModel &model, const CountMessage &countMessage) {
        update_visit(model.countModel, countMessage, overloaded {
            updateIncrementBy,
            updateDecrementBy
        });
};



auto updateCameraModle = [](AppModel &model, const CameraMessage &cameraMessage) {
        update_visit(model.cameraConnectedModel, cameraMessage, overloaded {
            updateCameraConnected,
            updateCameraDisconnected
        });
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
        update_visit(_model, message, overloaded {
            updateCountModel,
            updateCameraModle
        });   
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


