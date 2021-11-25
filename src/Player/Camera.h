#ifndef CAMERA_H
#define CAMERA_H

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

class Camera
{
private:
	glm::vec3 m_Location;
	float m_Yaw;
	float m_Pitch;

	float m_Fov;

	const glm::vec3 m_WorldUp{ 0.0, 1.0, 0.0 };
	glm::vec3 m_Up;
	glm::vec3 m_Front;
	glm::vec3 m_Right;

	GLFWwindow* m_Win;

public:
	Camera(glm::vec3 location, float yaw, float pitch, float fov, GLFWwindow* win);

	Camera();

	glm::mat4 getProjectionMat() const;

	glm::mat4 getViewMat() const;

	void calculateVecs();

	void handleMouse(glm::vec2 offset);

	void handleKeyboard(glm::vec3 velocity, float dt);

	glm::vec3 getLocation() const;

	float getYaw() const;

	float getPitch() const;

	glm::vec3 getFront() const;

	glm::vec3 getRight() const;

	glm::vec3 getWorldUp() const;

	void setLocation(glm::vec3 loc);
};

#endif