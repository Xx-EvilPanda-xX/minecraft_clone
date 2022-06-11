#ifndef VECTOR2I_H
#define VECTOR2I_H

#include "Vector3i.h"

class Vector2i
{
public:
	int x;
	int y;

	Vector2i(int x, int y);

	Vector2i(Vector3i vec);

	Vector2i();

	friend bool operator== (const Vector2i& v1, const Vector2i& v2);
	friend bool operator!= (const Vector2i& v1, const Vector2i& v2);

	friend bool operator< (const Vector2i& v1, const Vector2i& v2);
	friend bool operator> (const Vector2i& v1, const Vector2i& v2);
};

#endif
