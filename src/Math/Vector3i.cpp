#include "Vector3i.h"

Vector3i::Vector3i(int x, int y, int z) : x{ x }, y{ y }, z{ z }
{
}

Vector3i::Vector3i()
{
	x = 0;
	y = 0;
	z = 0;
}