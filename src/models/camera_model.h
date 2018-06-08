
#ifndef SRC_MODELS_CAMERA_MODEL_H
#define SRC_MODELS_CAMERA_MODEL_H

struct CameraConnectedModel
{
	bool isConnected = false;
};

bool operator==(const CameraConnectedModel& left, const CameraConnectedModel& right)
{
	return left.isConnected == right.isConnected;
}

#endif  // SRC_MODELS_CAMERA_MODEL_H