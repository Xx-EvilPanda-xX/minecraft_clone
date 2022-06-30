#ifndef PLAYER_H
#define PLAYER_H

#include <random>
#include <glm/glm.hpp>
#include "Camera.h"
#include "../World/ChunkManager.h"
#include "../Math/Vector3i.h"
#include "../Input/Keyboard.h"

enum class CollisionType
{
	PlayerUpperHalf,
	PlayerLowerHalf
};

class Player
{
private:
	Camera m_Camera;
	glm::dvec3 m_Velocity;
	AABB m_Aabb;

	ChunkManager& m_Manager;
	Keyboard& m_Keyboard;

	glm::dvec3 m_LastValidLoc;
	AABB m_LastValidAABB;
	
	glm::bvec3 m_LastMoved;
	glm::bvec3 m_DecreasingVel;

	bool m_Sprinting;
	bool m_Grounded;
	bool m_Flying;
	bool m_HasTouchedGround;
	double m_Reach;
	double m_JumpCoolDown;

	const double vecPrecision{ 0.005 };
	const double cameraHeightDiff{ 0.3 };
	const int collisionSearchRadiusX{ 2 };
	const int collisionSearchRadiusY{ 3 };
	const int collisionSearchRadiusZ{ 2 };
	const double worldBottom{ -40.0 };

	template <typename T>
	T* intersect(bool throughFoliage);

	void updateMeshes(Vector3i editPos);

	void calculateVelocity();

	void collisionDetection();

	bool collide(glm::dvec3 playerLowerHalf, glm::dvec3 playerUpperHalf, const AABB& playerAABB, Vector3i& o_Pos, CollisionType& o_CollisionType);

	Vector3i* test(glm::dvec3 playerPos, const AABB& playerAABB, double& o_ClosestCollision);

	AABB createPlayerAABB(glm::dvec3 playerPos);

	void buildUpdatedMesh(Chunk* chunk);

public:
	Player(ChunkManager& manager, Keyboard& keyboard, double reach);

	void move();

	void breakBlock();

	void placeBlock(BlockType block);

	Camera& getCamera();

	glm::dvec3& getVelocity();

	double getReach() const;

	bool isSprinting() const;

	bool isGrounded() const;

	bool isFlying() const;

	bool isHasTouchedGround() const;

	glm::bvec3& getDecreasingVel();

	void setReach(double reach);

	void setSprinting(bool sprinting);

	void setFlying(bool flying);

	void setHasTouchedGround(bool hasTouchedGround);
};

#endif