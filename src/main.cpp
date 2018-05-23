
#include <variant>
#include <vector>
#include <iostream>
#include <string>
#include <cxxabi.h>


#include "actions/app_actions.h"
#include "models/app_model.h"
#include "reducers/app_reducers.h"
#include "elmish.h"







using AppStore = Store<AppModel, AppAction>;


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


