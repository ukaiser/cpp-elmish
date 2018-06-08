
#ifndef SRC_REDUCERS_CAMERA_REDUCERS_H
#define SRC_REDUCERS_CAMERA_REDUCERS_H

#include <string>
#include "actions/count_actions.h"
#include "elmish.h"
#include "models/app_model.h"

auto updateCameraConnected = [](CameraConnectedModel& model, const CameraConnected&) {
	model.isConnected = true;
};

auto updateCameraDisconnected = [](CameraConnectedModel& model, const CameraDisconnected&) {
	model.isConnected = false;
};

auto updateCameraModle = [](AppModel& model, const CameraAction& cameraMessage) {
	updateVisit(model.cameraConnectedModel,
				cameraMessage,
				overloaded{updateCameraConnected, updateCameraDisconnected});
};

std::string actionToString(const CameraConnected&)
{
	return "cameraConnected";
}

std::string actionToString(const CameraDisconnected&)
{
	return "cameraDisconnected";
}

#endif  // SRC_REDUCERS_CAMERA_REDUCERS_H