
#ifndef SRC_REDUCERS_COUNT_REDUCERS_H
#define SRC_REDUCERS_COUNT_REDUCERS_H

#include <string>
#include "actions/count_actions.h"
#include "elmish.h"
#include "fmt/format.h"
#include "models/app_model.h"

auto updateIncrementBy = [](AppModel& model, const IncrementBy& incrementBy) {
	selectCountModel(model).counter += incrementBy.value;
};

auto updateDecrementBy = [](AppModel& model, const DecrementBy& decrementtBy) {
	selectCountModel(model).counter -= decrementtBy.value;
};

auto updateCountModel = [](AppModel& model, const CountAction& countMessage) {
	updateVisit(model, countMessage, overloaded{updateIncrementBy, updateDecrementBy});
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