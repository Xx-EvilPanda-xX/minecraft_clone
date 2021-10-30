#include <iostream>
#include "Window.h"

Window::Window(int width, int height, const char* title) : m_Width{ width }, m_Height{ height }, m_Title{ title }
{
	initGlfw();
	createWindow();
	loadGL();
}

Window::Window()
{

}

Window::~Window()
{

}

void Window::initGlfw()
{
	if (!glfwInit())
	{
		std::cout << "Failed to initialize GLFW! Program will exit.\n";
		std::exit(-1);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void Window::createWindow()
{
	m_Window = glfwCreateWindow(m_Width, m_Height, m_Title, NULL, NULL);
	if (m_Window == NULL)
	{
		std::cout << "Failed to create GLFW window! Program will exit.\n";
		glfwTerminate();
		std::exit(-1);
	}
	glfwMakeContextCurrent(m_Window);

	glfwSetFramebufferSizeCallback(m_Window, framebuffer_size_callback);
}

void Window::loadGL()
{
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to load OpenGL! Program will exit.\n";
		std::exit(-1);
	}
}

GLFWwindow* Window::getWindow()
{
	return m_Window;
}

int Window::getWidth()
{
	return m_Width;
}

int Window::getHeight()
{
	return m_Height;
}

const char* Window::getTitle()
{
	return m_Title;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
