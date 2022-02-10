#ifndef PLAYER_H
#define PLAYER_H

#include <random>
#include <glm/glm.hpp>
#include "Camera.h"
#include "../World/ChunkManager.h"
#include "../Math/Vector3i.h"

enum class CollsionType
{
	PlayerUpperHalf,
	PlayerLowerHalf
};

class Player
{
private:
	Camera& m_Cam;
	ChunkManager& m_Manager;
	glm::dvec3 m_Velocity;
	AABB m_Aabb;

	glm::dvec3 m_LastValidLoc;
	AABB m_LastValidAABB;

	bool m_LastMovedX;
	bool m_LastMovedY;
	bool m_LastMovedZ;

	bool m_Sprinting;
	bool m_Grounded;
	bool m_Flying;
	double m_Reach;

	const double vecPrecision{ 0.001 };

	Vector3i* breakIntersect();

	glm::dvec3* placeIntersect();

	void updateMeshes(Vector3i editPos);

	void calculateVelocity();

	bool testCollide(glm::dvec3 playerLowerHalf, glm::dvec3 playerUpperHalf, AABB& playerAABB, Vector3i& o_Pos, CollsionType& o_CollisionType);

	AABB createPlayerAABB(glm::dvec3 playerPos);

public:
	Player(Camera& cam, ChunkManager& manager, double reach);

	void move();

	void breakBlock();

	void placeBlock(BlockType block);

	double getReach() const;

	glm::dvec3& getVelocity();

	bool isSprinting();

	bool isGrounded();

	bool isFlying();

	void setReach(double reach);

	void setSprinting(bool sprinting);

	void setFlying(bool flying);
};

#endif