#ifndef PLAYER_H
#define PLAYER_H

#include <glm/glm.hpp>
#include "Camera.h"
#include "../World/ChunkManager.h"
#include "../Math/Vector3i.h"

class Player
{
private:
	Camera& m_Cam;
	ChunkManager* m_Manager;
	glm::vec3 m_Velocity;
	AABB m_Aabb;

	glm::vec3 m_LastValidLoc;
	AABB m_LastValidAABB;
	
	bool m_LastCollideX;
	bool m_LastCollideY;
	bool m_LastCollideZ;

	bool m_Sprinting;
	float m_Reach;

	Vector3i* breakIntersect();

	glm::vec3* placeIntersect();

	Vector3i sectionLocal(Vector3i worldPos);

	void updateMeshes(Vector3i editPos);

	void calculateVelocity();

	bool testCollide(glm::vec3 playerPos, AABB& playerAABB, Vector3i& o_Pos, AABB& o_Aabb);

	bool testCollide(glm::vec3 playerPos, AABB& playerAABB);

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
	
	void setSprinting(bool sprinting);
};

#endif