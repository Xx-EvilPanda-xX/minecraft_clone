#include "Keyboard.h"

Keyboard::Keyboard(GLFWwindow* win)
{
	this->m_Win = win;
}

Keyboard::Keyboard() = default;

bool Keyboard::isKeyDown(int key) const
{
	return glfwGetKey(m_Win, key) == GLFW_PRESS;
}