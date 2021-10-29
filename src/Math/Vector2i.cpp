#include "Vector2i.h"

Vector2i::Vector2i(int x, int y) : m_X{ x }, m_Y{ y }
{
}

Vector2i::Vector2i()
{
	m_X = 0;
	m_Y = 0;
}