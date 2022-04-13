#include "Vector3i.h"

Vector3i::Vector3i(int x, int y, int z) 
	: x{ x },
	y{ y },
	z{ z }
{
}

Vector3i::Vector3i(const glm::dvec3& vec)
{
	x = static_cast<int>(vec.x); 
	y = static_cast<int>(vec.y);
	z = static_cast<int>(vec.z);

	if (vec.x < 0.0)
		x -= 1;

	if (vec.y < 0.0)
		y -= 1;

	if (vec.z < 0.0)
		z -= 1;
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

bool operator< (const Vector3i& v1, const Vector3i& v2)
{
	return (v1.x < v2.x) && (v1.y < v2.y) && (v1.z < v2.z);
}

bool operator> (const Vector3i& v1, const Vector3i& v2)
{
	return (v1.x > v2.x) && (v1.y > v2.y) && (v1.z > v2.z);
}