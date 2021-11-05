#include <cmath>
#include "Camera.h"
#include "../Constants.h"

Camera::Camera(glm::vec3 location, float yaw, float pitch, float fov) : m_Location{ location }, m_Yaw{ yaw }, m_Pitch{ pitch }, m_Fov{ fov }
{
	calculateVecs();
}

Camera::Camera() = default;

Camera::~Camera()
{
}

glm::mat4 Camera::getProjectionMat() const
{
	return glm::perspective(glm::radians(m_Fov), static_cast<float>(constants::windowWidth) / static_cast<float>(constants::windowHeight), 0.1f, 1000.0f);
}

glm::mat4 Camera::getViewMat() const
{
	return glm::lookAt(m_Location, m_Location + m_Front, m_Up);
}

void Camera::handleMouse(glm::vec2 offset, float dt)
{
	m_Yaw += offset.x * constants::mouse_sensitivity * dt;
	m_Pitch += offset.y * constants::mouse_sensitivity * dt;

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

void Camera::handleKeyboard(Direction dir, float velocity, float dt)
{
	velocity *= dt;
	
	switch (dir)
	{
	case Direction::Up:
		m_Location += m_WorldUp * velocity;
		break;

	case Direction::Down:
		m_Location -= m_WorldUp * velocity;
		break;

	case Direction::Right:
		m_Location += m_Right * velocity;
		break;

	case Direction::Left:
		m_Location -= m_Right * velocity;
		break;

	case Direction::Forward:
		m_Location += glm::vec3(m_Front.x, 0.0f, m_Front.z) * velocity;
		break;
	case Direction::Back:
		m_Location -= glm::vec3(m_Front.x, 0.0f, m_Front.z) * velocity;
		break;
	}

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