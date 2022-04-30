#ifndef RANDOM_H
#define RANDOM_H

#include <random>

class Random
{
private:
	std::mt19937 m_Rand;
	std::uniform_int_distribution<> m_Die;
public:
	Random(int bottomLimit, int topLimit);

	~Random();

	void setSeed(int s);

	int get();
};

#endif
