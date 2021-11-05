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

bool operator== (const Vector3i& v1, const Vector3i& v2)
{
	return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z;
}

bool operator!= (const Vector3i& v1, const Vector3i& v2)
{
	return v1.x != v2.x || v1.y != v2.y || v1.z != v2.z;
}
