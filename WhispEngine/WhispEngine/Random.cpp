#include "Random.h"

pcg32 Random::rng;
Random::Random()
{
	rng = pcg32(seed_source);
	guid = std::uniform_real_distribution<double>(1, UINT64_MAX); // starts with 1 to keep 0 as invalid
}


Random::~Random()
{
}

float Random::Randomf(const float & min, const float & max)
{
	std::uniform_real_distribution<float> rand(min, max);
	
	return rand(rng);
}

int Random::Randomi(const int & min, const int & max)
{
	std::uniform_real_distribution<double> rand(min, max);
	return rand(rng);
}

uint64_t Random::RandomGUID()
{
	return guid(rng);
}
