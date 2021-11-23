#ifndef AABB_H
#define AABB_H

#include <glm/vec3.hpp>

class AABB
{
private:
	glm::vec3 m_Min;
	glm::vec3 m_Max;

public:
	AABB(glm::vec3 min, glm::vec3 max);

	AABB();

	bool intersects(const AABB& other);

	glm::vec3 min() const;

	glm::vec3 max() const;

	void min(glm::vec3 v);

	void max(glm::vec3 v);
};

#endif