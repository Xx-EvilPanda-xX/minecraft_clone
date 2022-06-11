#include "Vector2i.h"

Vector2i::Vector2i(int x, int y) 
	: x{ x },
	y{ y }
{
}

Vector2i::Vector2i(Vector3i vec)
{
	x = vec.x / 16;
	y = vec.z / 16;

	if (vec.x < 0 && vec.x % 16 != 0)
		--x;

	if (vec.z < 0 && vec.z % 16 != 0)
		--y;
}

Vector2i::Vector2i()
{
	x = 0;
	y = 0;
}

bool operator== (const Vector2i& v1, const Vector2i& v2)
{
	return v1.x == v2.x && v1.y == v2.y;
}

bool operator!= (const Vector2i& v1, const Vector2i& v2)
{
	return v1.x != v2.x || v1.y != v2.y;
}

bool operator< (const Vector2i& v1, const Vector2i& v2)
{
	return (v1.x < v2.x) && (v1.y < v2.y);
}

bool operator> (const Vector2i& v1, const Vector2i& v2)
{
	return (v1.x > v2.x) && (v1.y > v2.y);
}
