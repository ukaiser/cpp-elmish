
#ifndef SRC_REDUCERS_APP_REDUCERS_H
#define SRC_REDUCERS_APP_REDUCERS_H

#include "camera_reducers.h"
#include "count_reducers.h"

void updateModel(AppModel& model, const AppAction& message)
{
	updateVisit(model, message, overloaded{updateCountModel, updateCameraModle});
}

#endif  // SRC_REDUCERS_APP_REDUCERS_H