#ifndef CAMERA_H
#define CAMERA_H

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

class Camera
{
private:
	glm::vec3 m_Location;
	float m_Yaw;
	float m_Pitch;

	const glm::vec3 worldUp{ 0.0, 1.0, 0.0 };
	glm::vec3 up;
	glm::vec3 front;
	glm::vec3 right;

	void handleMouse();
	 
	void handleKeyBoard();

	void calculateVecs();

public:
	Camera(glm::vec3 location, float yaw, float pitch);

	~Camera();

	glm::mat4 getProjectionMat();

	glm::mat4 getViewMat();

	void update();
};

#endif