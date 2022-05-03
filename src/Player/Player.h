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
	Camera m_Camera;
	glm::dvec3 m_Velocity;
	AABB m_Aabb;

	ChunkManager& m_Manager;
	Keyboard& m_Keyboard;

	glm::dvec3 m_LastValidLoc;
	AABB m_LastValidAABB;

	bool m_LastMovedX;
	bool m_LastMovedY;
	bool m_LastMovedZ;

	bool m_Sprinting;
	bool m_Grounded;
	bool m_Flying;
	bool m_CtrlLastDown;
	bool m_DecreasingVel;
	double m_Reach;
	double m_JumpCoolDown;

	const double vecPrecision{ 0.001 };
	const double cameraHeightDiff{ 0.3 };
	const int collsionSearchRadiusX{ 2 };
	const int collsionSearchRadiusY{ 3 };
	const int collsionSearchRadiusZ{ 2 };
	const double worldBottom{ -40.0 };

	template <typename T>
	T* intersect();

	void updateMeshes(Vector3i editPos);

	void calculateVelocity();

	void collsionDetection();

	bool collide(glm::dvec3 playerLowerHalf, glm::dvec3 playerUpperHalf, const AABB& playerAABB, Vector3i& o_Pos, CollsionType& o_CollisionType);

	Vector3i* test(glm::dvec3 playerPos, const AABB& playerAABB, double& o_ClosestCollision);

	AABB createPlayerAABB(glm::dvec3 playerPos);

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

	void setReach(double reach);

	void setSprinting(bool sprinting);

	void setFlying(bool flying);
};

#endif