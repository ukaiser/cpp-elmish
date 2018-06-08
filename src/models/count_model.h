#ifndef SRC_MODEL_COUNT_MODEL_H
#define SRC_MODEL_COUNT_MODEL_H

struct CountModel
{
	int counter = 0;
};

bool operator==(const CountModel& left, const CountModel& right)
{
	return left.counter == right.counter;
}

#endif  // SRC_MODEL_COUNT_MODEL_H