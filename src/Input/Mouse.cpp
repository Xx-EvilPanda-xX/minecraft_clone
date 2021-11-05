#include "Mouse.h"

Mouse::Mouse() = default;

Mouse::~Mouse()
{
}

float Mouse::getXOffset()
{
	if (usedX)
	{
		return 0.0f;
	}

	usedX = true;
	return m_xOffset;
}

float Mouse::getYOffset()
{
	if (usedY)
	{
		return 0.0f;
	}

	usedY = true;
	return m_yOffset;
}

void Mouse::setXOffset(float x)
{
	m_xOffset = x;
	usedX = false;
}

void Mouse::setYOffset(float y)
{
	m_yOffset = -y;
	usedY = false;
}