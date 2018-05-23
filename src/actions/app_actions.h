
#ifndef SRC_ACTIONS_APP_ACTIONS_H
#define SRC_ACTIONS_APP_ACTIONS_H

#include "camera_actions.h"
#include "count_actions.h"

using AppAction = std::variant<CountAction, CameraAction>;

#endif  // SRC_ACTIONS_APP_ACTIONS_H