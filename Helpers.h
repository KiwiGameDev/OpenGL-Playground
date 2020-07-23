#pragma once
#include <stdlib.h>

inline float randf()
{
	return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}