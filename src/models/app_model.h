
#ifndef SRC_MODELS_APP_MODEL_H
#define SRC_MODELS_APP_MODEL_H

#include "camera_model.h"
#include "count_model.h"


struct AppModel {
  CountModel countModel;
  CameraConnectedModel cameraConnectedModel;
};


CountModel& selectCountModel(AppModel& appModel)
{
    return appModel.countModel;
}

CameraConnectedModel& slectCameraModel(AppModel& appModel)
{
    return appModel.cameraConnectedModel;
}

#endif  // SRC_MODELS_APP_MODEL_H