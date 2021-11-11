#ifndef WINDOW_H
#define WINDOW_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Input/Keyboard.h"
#include "Input/Mouse.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_move_callback(GLFWwindow* window, double xpos, double ypos);

class Window
{
private:
	GLFWwindow* m_Window;
	int m_Width;
	int m_Height;
	const char* m_Title;

	void initGlfw();
	void createWindow();
	void loadGL();

public:
	Window(int width, int height, const char* title);

	Window();

	GLFWwindow* getWindow();

	int getWidth();

	int getHeight();

	const char* getTitle();

	Keyboard& getKeyboard();

	Mouse& getMouse();

	void setTitle(const char* title);
};

#endif