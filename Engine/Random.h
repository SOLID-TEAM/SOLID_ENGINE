#ifndef _RANDOM_H__
#define _RANDOM_H__

#include <random>
#include "PCG/include/pcg_random.hpp"

class Random
{
public:

	static int GetIntRandomValue(int range_i1, int range_i2)
	{
		pcg_extras::seed_seq_from<std::random_device> seed_source;
		pcg32 rng(seed_source);
		std::uniform_int_distribution<int> uniform_dist1(range_i1, range_i2);
		int mean = uniform_dist1(rng);

		return mean;
	}

	static float GetFloatRandomValue(float range_f1, float range_f2)
	{
		pcg_extras::seed_seq_from<std::random_device> seed_source;
		pcg32 rng(seed_source);
		std::uniform_real_distribution<float> uniform_dist(range_f1, range_f2);
		float mean = uniform_dist(rng);
		return mean;
	}

	// Return a value between 0 and 1

	static float GetRandomPercent()
	{
		pcg_extras::seed_seq_from<std::random_device> seed_source;
		pcg32 rng(seed_source);
		std::uniform_real_distribution<float> uniform_dist(0, 1);
		float mean = uniform_dist(rng);
		return mean;
	}
};

#endif // !_RANDOM_H__

