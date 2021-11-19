#include "Mouse.h"

Mouse::Mouse(GLFWwindow* win)
	: m_Win{ win }
{
}

Mouse::Mouse() = default;

float Mouse::getXOffset()
{
	if (m_UsedX)
	{
		return 0.0f;
	}

	m_UsedX = true;
	return m_xOffset;
}

float Mouse::getYOffset()
{
	if (m_UsedY)
	{
		return 0.0f;
	}

	m_UsedY = true;
	return m_yOffset;
}

void Mouse::setXOffset(float x)
{
	m_xOffset = x;
	m_UsedX = false;
}

void Mouse::setYOffset(float y)
{
	m_yOffset = -y;
	m_UsedY = false;
}

bool Mouse::isButtonDown(int button)
{
	return glfwGetMouseButton(m_Win, button) == GLFW_PRESS;
}