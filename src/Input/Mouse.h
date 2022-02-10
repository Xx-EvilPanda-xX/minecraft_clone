#ifndef MOUSE_H
#define MOUSE_H

#include <GLFW/glfw3.h>

class Mouse
{
private:
	double m_xOffset;
	double m_yOffset;

	bool m_UsedX;
	bool m_UsedY;

	GLFWwindow* m_Win;

public:

	Mouse(GLFWwindow* win);

	Mouse();

	double getXOffset();

	double getYOffset();

	void setXOffset(double x);

	void setYOffset(double y);

	bool isButtonDown(int button);
};

#endif
