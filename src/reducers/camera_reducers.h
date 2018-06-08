
#ifndef SRC_REDUCERS_CAMERA_REDUCERS_H
#define SRC_REDUCERS_CAMERA_REDUCERS_H

#include <string>
#include "actions/count_actions.h"
#include "models/app_model.h"
#include "overloaded.h"

auto updateCameraConnected = [](AppModel& model, const CameraConnected&) {
	model.cameraConnectedModel.isConnected = true;
};

auto updateCameraDisconnected = [](AppModel& model, const CameraDisconnected&) {
	model.cameraConnectedModel.isConnected = false;
};

auto cameraReducers = overloaded{updateCameraConnected, updateCameraDisconnected};

std::string actionToString(const CameraConnected&)
{
	return "cameraConnected";
}

std::string actionToString(const CameraDisconnected&)
{
	return "cameraDisconnected";
}

#endif  // SRC_REDUCERS_CAMERA_REDUCERS_H