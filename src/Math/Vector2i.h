#ifndef VECTOR2I_H
#define VECTOR2I_H

class Vector2i
{
public:
	int x;
	int y;

	Vector2i(int x, int y);

	Vector2i();

	friend bool operator== (const Vector2i& v1, const Vector2i& v2);
	friend bool operator!= (const Vector2i& v1, const Vector2i& v2);
};

#endif
