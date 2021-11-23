#ifndef VECTOR3I_H
#define VECTOR3I_H

class Vector3i
{
public:
	int x;
	int y;
	int z;

	Vector3i(int x, int y, int z);

	Vector3i();

	friend bool operator== (const Vector3i& v1, const Vector3i& v2);
	friend bool operator!= (const Vector3i& v1, const Vector3i& v2);

	friend bool operator< (const Vector3i& v1, const Vector3i& v2);
	friend bool operator> (const Vector3i& v1, const Vector3i& v2);
};

#endif
