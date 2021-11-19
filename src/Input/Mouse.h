#ifndef MOUSE_H
#define MOUSE_H

#include <GLFW/glfw3.h>

class Mouse
{
private:
	float m_xOffset;
	float m_yOffset;

	bool m_UsedX;
	bool m_UsedY;

	GLFWwindow* m_Win;

public:

	Mouse(GLFWwindow* win);

	Mouse();

	float getXOffset();

	float getYOffset();

	void setXOffset(float x);

	void setYOffset(float y);

	bool isButtonDown(int button);
};

#endif
