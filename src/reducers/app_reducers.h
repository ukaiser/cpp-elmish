
#ifndef SRC_REDUCERS_APP_REDUCERS_H
#define SRC_REDUCERS_APP_REDUCERS_H

#include "count_reducers.h"
#include "camera_reducers.h"

void updateModel(AppModel &model, const AppAction& message)
{
    update_visit(model, message, overloaded {
        updateCountModel,
        updateCameraModle
    });   
}   

#endif  // SRC_REDUCERS_APP_REDUCERS_H