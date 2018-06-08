
#ifndef SRC_REDUCERS_APP_REDUCERS_H
#define SRC_REDUCERS_APP_REDUCERS_H

#include "camera_reducers.h"
#include "count_reducers.h"

auto appReducers = overloaded{cameraReducers, countReducers};

bool updateModel(AppModel& model, const AppAction& message)
{
	return updateVisit(model, message, appReducers);
}
#endif  // SRC_REDUCERS_APP_REDUCERS_H