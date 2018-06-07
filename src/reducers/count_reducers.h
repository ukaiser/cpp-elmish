
#ifndef SRC_REDUCERS_COUNT_REDUCERS_H
#define SRC_REDUCERS_COUNT_REDUCERS_H

#include <string>
#include "fmt/format.h"
#include "elmish.h"
#include "models/app_model.h"
#include "actions/count_actions.h"


#if 0
auto updateIncrementBy = [](AppModel &model, const IncrementBy &incrementBy) {
  selectCountModel(model).counter += incrementBy.value;
};
#else
struct UpdateIncrementBy
{
    void operator()(AppModel &model, const IncrementBy &incrementBy) const {
        selectCountModel(model).counter += incrementBy.value;
    }
};
auto updateIncrementBy = UpdateIncrementBy{};
#endif



auto updateDecrementBy = [](AppModel &model, const DecrementBy &decrementtBy) {
  selectCountModel(model).counter -= decrementtBy.value;
};



auto updateCountModel = [](AppModel &model, const CountAction &countMessage) {
    updateVisit(model, countMessage, overloaded {
            updateIncrementBy,
            updateDecrementBy
    });
};

std::string actionToString(const IncrementBy& incrementBy)
{
    return fmt::format("IncrementBy {}", incrementBy.value);
} 

std::string actionToString(const DecrementBy& decrementBy)
{
    return fmt::format("DecrementBy {}", decrementBy.value);
}


#endif  // SRC_REDUCERS_COUNT_REDUCERS_H