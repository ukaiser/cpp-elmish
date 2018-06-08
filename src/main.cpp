
#include <iostream>
#include <string>
#include <variant>
#include <vector>

#include "actions/app_actions.h"
#include "elmish.h"
#include "models/app_model.h"
#include "pybind11/embed.h"
#include "pybind11/functional.h"
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"
#include "reducers/app_reducers.h"

namespace py = pybind11;

using AppStore = Store<AppModel, AppAction>;

struct AppStoreAdapter
{
	void update(const AppAction& action) { AppStore::instance().update(action); }

	void subscribeModelCallback(const std::function<void(const AppModel&)>& c)
	{
		AppStore::instance().subscribe(c);
	}

	void setReduceCallback(const AppStore::ReduceCallback& reduceCallback)
	{
		AppStore::instance().setReduceCallback(reduceCallback);
	}
};

PYBIND11_EMBEDDED_MODULE(elmish, m)
{
	py::class_<CountModel>(m, "CountModel").def_readwrite("counter", &CountModel::counter);

	py::class_<AppModel>(m, "AppModel").def_readwrite("countModel", &AppModel::countModel);

	py::class_<CameraConnected>(m, "CameraConnected")
		.def(py::init<bool>())
		.def_readwrite("connected", &CameraConnected::connected);

	py::class_<IncrementBy>(m, "IncrementBy")
		.def(py::init<int>())
		.def_readwrite("value", &IncrementBy::value);

	py::class_<AppStoreAdapter>(m, "AppStoreAdapter")
		.def(py::init<>())
		.def("update", &AppStoreAdapter::update)
		.def("subscribeModelCallback", &AppStoreAdapter::subscribeModelCallback)
		.def("setReduceCallback", &AppStoreAdapter::setReduceCallback);
}

int main()
{
	AppStoreAdapter appStoreAdapter;
	auto test = [](const IncrementBy& incrementBy, const AppModel& model) {
		std::cout << "IncremtnBy Subscription called: " << incrementBy.value
				  << " to: " << model.countModel.counter << "\n";
	};
	AppStore::instance().subscribe<IncrementBy>(test);

	AppStore::instance().subscribe<CameraConnected>(
		[](auto&&) { std::cout << "CameraConnected! \n"; });
	AppStore::instance().subscribe<CameraDisconnected>(
		[] { std::cout << "CameraDisconnected! \n"; });

	AppStore::instance().subscribe([](const AppModel& model) {
		std::cout << "{\n";
		std::cout << "  counter : " << model.countModel.counter << ";\n";
		std::cout << "  isConnected : " << model.cameraConnectedModel.isConnected << ";\n";
		std::cout << "};\n";
	});

	AppStore::instance().update(IncrementBy{2});
	AppStore::instance().update(IncrementBy{3});

	appStoreAdapter.update(CameraConnected{});

	AppStore::instance().update(DecrementBy{4});
	AppStore::instance().update(CameraDisconnected{});

	py::scoped_interpreter guard{};  // start the interpreter and keep it alive

	py::exec(R"(
	import elmish

	def myModelPrint(model):
		print('AppModel counter: {}'.format(model.countModel.counter))	

	
	def reduce(model, action):
		print('Action type: {}'.format(type(action)))
		
		if isinstance(action, elmish.IncrementBy):
			model.countModel.counter += action.value
		
		return model

	

	connected = elmish.CameraConnected(False)

	appStoreAdapter = elmish.AppStoreAdapter()
	appStoreAdapter.subscribeModelCallback(myModelPrint)
	appStoreAdapter.setReduceCallback(reduce)
	appStoreAdapter.update(elmish.IncrementBy(42))

    )");

	AppStore::instance().update(IncrementBy{2});
	AppStore::instance().update(IncrementBy{3});
}
