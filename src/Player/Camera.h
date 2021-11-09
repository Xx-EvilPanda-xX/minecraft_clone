#ifndef CAMERA_H
#define CAMERA_H

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class Direction
{
	Up,
	Down,
	Right,
	Left,
	Forward,
	Back
};

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

	void calculateVecs();

public:
	Camera(glm::vec3 location, float yaw, float pitch, float fov);

	Camera();

	~Camera();

	glm::mat4 getProjectionMat() const;

	glm::mat4 getViewMat() const;

	void handleMouse(glm::vec2 offset);

	void handleKeyboard(Direction dir, float velocity, float dt);

	glm::vec3 getLocation() const;

	float getYaw() const;

	float getPitch() const;
};

#endif