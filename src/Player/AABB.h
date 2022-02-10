#ifndef AABB_H
#define AABB_H

#include <glm/vec3.hpp>

class AABB
{
private:
	glm::dvec3 m_Min;
	glm::dvec3 m_Max;

public:
	AABB(glm::dvec3 min, glm::dvec3 max);

	AABB();

	bool intersects(const AABB& other);

	glm::dvec3 min() const;

	glm::dvec3 max() const;

	void min(glm::dvec3 v);

	void max(glm::dvec3 v);
};

#endif