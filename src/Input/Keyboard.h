#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <GLFW/glfw3.h>

class Keyboard
{
private:
	GLFWwindow* m_Win;

public:
	Keyboard(GLFWwindow* win);

	Keyboard();

	bool isKeyDown(int key) const;
};

#endif
