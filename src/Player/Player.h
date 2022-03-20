#ifndef PLAYER_H
#define PLAYER_H

#include <random>
#include <glm/glm.hpp>
#include "Camera.h"
#include "../World/ChunkManager.h"
#include "../Math/Vector3i.h"
#include "../Input/Keyboard.h"

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
	Keyboard& m_Keyboard;

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
	const double cameraHeightDiff{ 0.35 };
	const int collsionSearchRadiusX{ 2 };
	const int collsionSearchRadiusY{ 3 };
	const int collsionSearchRadiusZ{ 2 };

	Vector3i* breakIntersect();

	glm::dvec3* placeIntersect();

	void updateMeshes(Vector3i editPos);

	void calculateVelocity();

	bool collide(glm::dvec3 playerLowerHalf, glm::dvec3 playerUpperHalf, const AABB& playerAABB, Vector3i& o_Pos, CollsionType& o_CollisionType);

	Vector3i* test(glm::dvec3 playerPos, const AABB& playerAABB, double& o_ClosestCollision);

	AABB createPlayerAABB(glm::dvec3 playerPos);

public:
	Player(Camera& cam, ChunkManager& manager, Keyboard& keyboard, double reach);

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