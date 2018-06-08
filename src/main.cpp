
#include <iostream>
#include <string>
#include <variant>
#include <vector>

#include "actions/app_actions.h"
#include "elmish.h"
#include "models/app_model.h"
#include "reducers/app_reducers.h"

using AppStore = Store<AppModel, AppAction>;

int main()
{
	AppStore appStore;
	auto test = [](const IncrementBy& incrementBy, const AppModel& model) {
		std::cout << "IncremtnBy Subscription called: " << incrementBy.value
				  << " to: " << model.countModel.counter << "\n";
	};
	appStore.subscribe<IncrementBy>(test);

	appStore.subscribe<CameraConnected>([](auto&&) { std::cout << "CameraConnected! \n"; });
	appStore.subscribe<CameraDisconnected>([] { std::cout << "CameraDisconnected! \n"; });

	appStore.subscribe([](const AppModel& model) {
		std::cout << "{\n";
		std::cout << "  counter : " << model.countModel.counter << ";\n";
		std::cout << "  isConnected : " << model.cameraConnectedModel.isConnected << ";\n";
		std::cout << "};\n";
	});

	appStore.update(IncrementBy{2});
	appStore.update(IncrementBy{3});
	appStore.update(CameraConnected{});
	appStore.update(DecrementBy{4});
	appStore.update(CameraDisconnected{});
}
