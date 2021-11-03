#include "Mouse.h"

Mouse::Mouse() = default;

Mouse::~Mouse()
{
}

void Mouse::update()
{
	m_xOffset = m_xPos - m_LastxPos;
	m_yOffset = m_yPos - m_LastyPos;
}

float Mouse::getXOffset()
{
	return m_xOffset;
}

float Mouse::getYOffset()
{
	return m_yOffset;
}

void Mouse::setX(float x)
{
	m_LastxPos = m_xPos;
	m_xPos = x;
}

void Mouse::setY(float y)
{
	m_LastyPos = m_yPos;
	m_yPos = y;
}