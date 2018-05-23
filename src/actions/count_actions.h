
#ifndef SRC_ACTIONS_COUNT_ACTIONS_H
#define SRC_ACTIONS_COUNT_ACTIONS_H

#include <variant>

struct IncrementBy
{
	int value;
};

struct DecrementBy
{
	int value;
};

using CountAction = std::variant<IncrementBy, DecrementBy>;

#endif  // SRC_ACTIONS_COUNT_ACTIONS_H