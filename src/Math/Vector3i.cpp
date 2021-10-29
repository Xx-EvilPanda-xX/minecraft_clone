#include "Vector3i.h"

Vector3i::Vector3i(int x, int y, int z) : m_X{ x }, m_Y{ y }, m_Z{ z }
{
}

Vector3i::Vector3i()
{
	m_X = 0;
	m_Y = 0;
	m_Z = 0;
}