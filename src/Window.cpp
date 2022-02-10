#include <iostream>
#include "Window.h"
#include "Constants.h"

Keyboard g_Keyboard;
Mouse g_Mouse;

Window::Window(int width, int height, const char* title) 
	: m_Width{ width },
	m_Height{ height },
	m_Title{ title }
{
	initGlfw();
	createWindow();
	loadGL();

	g_Keyboard = Keyboard{ m_Window };
	g_Mouse = Mouse{ m_Window };
}

Window::Window() = default;

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
	if (constants::fullscreen)
	{
		m_Window = glfwCreateWindow(constants::fullscreenWidth, constants::fullscreenHeight, m_Title, glfwGetPrimaryMonitor(), NULL);
	}
	else
	{
		m_Window = glfwCreateWindow(m_Width, m_Height, m_Title, NULL, NULL);
	}
	
	if (m_Window == NULL)
	{
		std::cout << "Failed to create GLFW window! Program will exit.\n";
		glfwTerminate();
		std::exit(-1);
	}
	glfwMakeContextCurrent(m_Window);

	glfwSetFramebufferSizeCallback(m_Window, framebuffer_size_callback);
	glfwSetCursorPosCallback(m_Window, mouse_move_callback);

	glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Window::loadGL()
{
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to load OpenGL! Program will exit.\n";
		std::exit(-1);
	}
}

GLFWwindow* Window::getWindow() const
{
	return m_Window;
}

int Window::getWidth() const
{
	return m_Width;
}

int Window::getHeight() const
{
	return m_Height;
}

const char* Window::getTitle() const
{
	return m_Title;
}

Keyboard& Window::getKeyboard()
{
	return g_Keyboard;
}

Mouse& Window::getMouse()
{
	return g_Mouse;
}

void Window::setTitle(const char* title)
{
	m_Title = title;
	glfwSetWindowTitle(m_Window, m_Title);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_move_callback(GLFWwindow* window, double xpos, double ypos)
{
	g_Mouse.setXOffset(static_cast<double>(xpos));
	g_Mouse.setYOffset(static_cast<double>(ypos));
}
