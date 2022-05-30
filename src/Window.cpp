#include <iostream>
#include "Window.h"
#include "Constants.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_move_callback(GLFWwindow* window, double xpos, double ypos);

Window::Window(int width, int height, const char* title) 
	: m_Width{ width },
	m_Height{ height },
	m_Title{ title }
{
	initGlfw();
	createWindow();
	loadGL();

	s_Keyboard = Keyboard{ m_GlfwWindow };
	s_Mouse = Mouse{ m_GlfwWindow };
}

Window::Window() = default;

Keyboard Window::s_Keyboard{ nullptr };
Mouse Window::s_Mouse{ nullptr };

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
		m_GlfwWindow = glfwCreateWindow(constants::fullscreenWidth, constants::fullscreenHeight, m_Title, glfwGetPrimaryMonitor(), NULL);
	}
	else
	{
		m_GlfwWindow = glfwCreateWindow(m_Width, m_Height, m_Title, NULL, NULL);
	}
	
	if (m_GlfwWindow == NULL)
	{
		std::cout << "Failed to create GLFW window! Program will exit.\n";
		glfwTerminate();
		std::exit(-1);
	}
	glfwMakeContextCurrent(m_GlfwWindow);

	glfwSetFramebufferSizeCallback(m_GlfwWindow, framebuffer_size_callback);
	glfwSetCursorPosCallback(m_GlfwWindow, mouse_move_callback);

	glfwSetInputMode(m_GlfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (glfwRawMouseMotionSupported())
		glfwSetInputMode(m_GlfwWindow, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
}

void Window::loadGL()
{
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to load OpenGL! Program will exit.\n";
		std::exit(-1);
	}
}

GLFWwindow* Window::getGlfwWindow() const
{
	return m_GlfwWindow;
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
	return s_Keyboard;
}

Mouse& Window::getMouse()
{
	return s_Mouse;
}

void Window::setTitle(const char* title)
{
	m_Title = title;
	glfwSetWindowTitle(m_GlfwWindow, m_Title);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_move_callback(GLFWwindow* window, double xpos, double ypos)
{
	Window::getMouse().setXOffset(static_cast<double>(xpos));
	Window::getMouse().setYOffset(static_cast<double>(ypos));
}
