#include "Vector2i.h"

Vector2i::Vector2i(int x, int y) 
	: x{ x },
	y{ y }
{
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
