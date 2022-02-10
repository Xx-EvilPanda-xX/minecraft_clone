#include <cmath>
#include "Camera.h"
#include "../Constants.h"

Camera::Camera(glm::vec3 location, float yaw, float pitch, float fov, GLFWwindow* win)
	: m_Location{ location },
	m_Yaw{ yaw },
	m_Pitch{ pitch },
	m_Fov{ fov },
	m_Win{ win }
{
	calculateVecs();
}

Camera::Camera() = default;

glm::mat4 Camera::getProjectionMat() const
{
	int width, height;
	glfwGetWindowSize(m_Win, &width, &height);
	return glm::perspective(glm::radians(m_Fov), static_cast<float>(width) / static_cast<float>(height), 0.01f, 1000.0f);
}

glm::mat4 Camera::getViewMat() const
{
	return glm::lookAt(m_Location, m_Location + m_Front, m_Up);
}

void Camera::handleMouse(glm::vec2 offset)
{
	m_Yaw += offset.x * constants::mouse_sensitivity;
	m_Pitch += offset.y * constants::mouse_sensitivity;

	if (m_Pitch > 89.0f)
	{
		m_Pitch = 89.0f;
	}

	if (m_Pitch < -89.0f)
	{
		m_Pitch = -89.0f;
	}

	if (m_Yaw > 360.0f)
	{
		m_Yaw = 0.0f;
	}

	if (m_Yaw < 0.0f)
	{
		m_Yaw = 360.0f;
	}

	calculateVecs();
}

void Camera::handleKeyboard(glm::vec3 velocity, float dt)
{
	velocity *= dt;

	m_Location += glm::normalize(glm::vec3(m_Front.x, 0.0f, m_Front.z)) * velocity.z;
	m_Location += m_WorldUp * velocity.y;
	m_Location += m_Right * velocity.x;

	calculateVecs();
}

void Camera::calculateVecs()
{
	glm::vec3 front{};

	front.x = std::cos(glm::radians(m_Yaw)) * std::cos(glm::radians(m_Pitch));
	front.y = std::sin(glm::radians(m_Pitch));
	front.z = std::sin(glm::radians(m_Yaw)) * std::cos(glm::radians(m_Pitch));

	m_Front = glm::normalize(front);
	m_Right = glm::normalize(glm::cross(front, m_WorldUp));
	m_Up = glm::normalize(glm::cross(m_Right, front));
}

glm::vec3 Camera::getLocation() const
{
	return m_Location;
}

float Camera::getYaw() const
{
	return m_Yaw;
}

float Camera::getPitch() const
{
	return m_Pitch;
}

glm::vec3 Camera::getFront() const
{
	return m_Front;
}

void Camera::setLocation(glm::vec3 loc)
{
	m_Location = loc;
}