#include "Keyboard.h"

Keyboard::Keyboard(GLFWwindow* win)
{
	this->m_Win = win;
}

Keyboard::Keyboard()
{

}

Keyboard::~Keyboard()
{

}

bool Keyboard::isKeyDown(int key)
{
	return glfwGetKey(m_Win, key) == GLFW_PRESS;
}