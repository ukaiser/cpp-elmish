
#ifndef SRC_MODELS_CAMERA_ACTIONS_H
#define SRC_MODELS_CAMERA_ACTIONS_H


struct CameraConnected {};
struct CameraDisconnected {};

using CameraAction = std::variant<CameraConnected, CameraDisconnected>;
#endif  // SRC_MODELS_CAMERA_ACTIONS_H