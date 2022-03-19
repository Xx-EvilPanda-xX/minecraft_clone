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
	glm::dvec3 m_Location;
	double m_Yaw;
	double m_Pitch;

	double m_Fov;

	const glm::dvec3 m_WorldUp{ 0.0, 1.0, 0.0 };
	glm::dvec3 m_Up;
	glm::dvec3 m_Front;
	glm::dvec3 m_Right;

	GLFWwindow* m_Win;

public:
	Camera(glm::dvec3 location, double yaw, double pitch, double fov, GLFWwindow* win);

	Camera();

	glm::dmat4 getProjectionMat() const;

	glm::dmat4 getViewMat() const;

	void calculateVecs();

	void handleLook(glm::dvec2 offset);

	void handleMove(glm::dvec3 velocity, double dt);

	glm::dvec3 getLocation() const;

	double getYaw() const;

	double getPitch() const;

	glm::dvec3 getFront() const;

	void setLocation(glm::dvec3 loc);

	void setX(double x);

	void setY(double y);

	void setZ(double z);

};

#endif