#include <cmath>
#include "Camera.h"
#include "../Constants.h"

Camera::Camera(glm::dvec3 location, double yaw, double pitch, double fov, GLFWwindow* win)
	: m_Location{ location },
	m_Yaw{ yaw },
	m_Pitch{ pitch },
	m_Fov{ fov },
	m_Win{ win }
{
	calculateVecs();
}

Camera::Camera() = default;

glm::dmat4 Camera::getProjectionMat() const
{
	int width, height;
	glfwGetWindowSize(m_Win, &width, &height);
	return glm::perspective(glm::radians(m_Fov), static_cast<double>(width) / static_cast<double>(height), 0.01, 1000.0);
}

glm::dmat4 Camera::getViewMat() const
{
	return glm::lookAt(m_Location, m_Location + m_Front, m_Up);
}

void Camera::handleLook(glm::dvec2 offset)
{
	m_Yaw += offset.x * constants::mouse_sensitivity;
	m_Pitch += offset.y * constants::mouse_sensitivity;

	if (m_Pitch > 89.0)
	{
		m_Pitch = 89.0;
	}

	if (m_Pitch < -89.0)
	{
		m_Pitch = -89.0;
	}

	if (m_Yaw > 360.0)
	{
		m_Yaw = 0.0;
	}

	if (m_Yaw < 0.0)
	{
		m_Yaw = 360.0;
	}

	calculateVecs();
}

void Camera::handleMove(glm::dvec3 velocity, double dt)
{
	velocity *= dt;

	m_Location += glm::normalize(glm::dvec3(m_Front.x, 0.0, m_Front.z)) * velocity.z;
	m_Location += m_WorldUp * velocity.y;
	m_Location += m_Right * velocity.x;

	calculateVecs();
}

void Camera::calculateVecs()
{
	glm::dvec3 front{};

	front.x = std::cos(glm::radians(m_Yaw)) * std::cos(glm::radians(m_Pitch));
	front.y = std::sin(glm::radians(m_Pitch));
	front.z = std::sin(glm::radians(m_Yaw)) * std::cos(glm::radians(m_Pitch));

	m_Front = glm::normalize(front);
	m_Right = glm::normalize(glm::cross(front, m_WorldUp));
	m_Up = glm::normalize(glm::cross(m_Right, front));
}

glm::dvec3 Camera::getLocation() const
{
	return m_Location;
}

double Camera::getYaw() const
{
	return m_Yaw;
}

double Camera::getPitch() const
{
	return m_Pitch;
}

glm::dvec3 Camera::getFront() const
{
	return m_Front;
}

void Camera::setLocation(glm::dvec3 loc)
{
	m_Location = loc;
}

void Camera::setX(double x)
{
	m_Location.x = x;
}

void Camera::setY(double y)
{
	m_Location.y = y;
}

void Camera::setZ(double z)
{
	m_Location.z = z;
}