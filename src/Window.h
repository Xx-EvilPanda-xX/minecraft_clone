#ifndef WINDOW_H
#define WINDOW_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

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

	~Window();

	GLFWwindow* getWindow();

	int getWidth();

	int getHeight();

	const char* getTitle();
};

#endif