
#ifndef SRC_ACTIONS_APP_ACTIONS_H
#define SRC_ACTIONS_APP_ACTIONS_H

#include "camera_actions.h"
#include "count_actions.h"
#include "elmish.h"

using AppAction = details::flatten_variant_t<std::variant<CountAction, CameraAction>>;

#endif  // SRC_ACTIONS_APP_ACTIONS_H