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
	ChunkManager* m_Manager;
	glm::vec3 m_Velocity;
	AABB m_Aabb;

	std::mt19937 m_Rand;
	std::uniform_int_distribution<> m_Die;

	glm::vec3 m_LastValidLoc;
	AABB m_LastValidAABB;
	
	bool m_LastCollideX;
	bool m_LastCollideY;
	bool m_LastCollideZ;

	bool m_Sprinting;
	bool m_Grounded;
	bool m_Flying;
	float m_Reach;

	int m_Moves;

	Vector3i* breakIntersect();

	glm::vec3* placeIntersect();

	Vector3i sectionLocal(Vector3i worldPos);

	void updateMeshes(Vector3i editPos);

	void calculateVelocity();

	bool testCollide(glm::vec3 playerLowerHalf, glm::vec3 playerUpperHalf, AABB& playerAABB, Vector3i& o_Pos, CollsionType& o_CollisionType);

	AABB createPlayerAABB(glm::vec3 playerPos);

public:
	Player(Camera& cam, ChunkManager* manager, float reach);

	void move();

	void breakBlock();

	void placeBlock(BlockType block);

	float getReach() const;

	void setReach(float reach);

	void setManager(ChunkManager* manager);

	glm::vec3& getVelocity();

	bool isSprinting();

	bool isGrounded();

	bool isFlying();

	void setFlying(bool flying);
	
	void setSprinting(bool sprinting);
};

#endif