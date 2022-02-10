#include "AABB.h"

AABB::AABB(glm::dvec3 min, glm::dvec3 max)
	: m_Min{ min },
	m_Max{ max }
{
}

AABB::AABB()
{
}

bool operator< (const glm::dvec3& v1, const glm::dvec3& v2)
{
	return (v1.x < v2.x) && (v1.y < v2.y) && (v1.z < v2.z);
}

bool operator> (const glm::dvec3& v1, const glm::dvec3& v2)
{
	return (v1.x > v2.x) && (v1.y > v2.y) && (v1.z > v2.z);
}

bool AABB::intersects(const AABB& other)
{
	return (m_Min < other.max() && m_Max > other.min());
}

glm::dvec3 AABB::min() const
{
	return m_Min;
}

glm::dvec3 AABB::max() const
{
	return m_Max;
}

void AABB::min(glm::dvec3 v)
{
	m_Min = v;
}

void AABB::max(glm::dvec3 v)
{
	m_Max = v;
}
