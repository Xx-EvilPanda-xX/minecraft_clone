#include "Random.h"

Random::Random(int bottomLimit, int topLimit)
	: m_Rand{ static_cast<std::mt19937::result_type>(std::time(nullptr)) },
	m_Die{ bottomLimit, topLimit }
{
}

Random::~Random() = default;

void Random::setSeed(int s)
{
	m_Rand.seed(s);
}

void Random::setRange(int bottomLimit, int topLimit)
{
	m_Die = std::uniform_int_distribution<>{ bottomLimit, topLimit };
}

int Random::get()
{
	return m_Die(m_Rand);
}

