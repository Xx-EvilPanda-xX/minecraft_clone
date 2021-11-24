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
	glm::vec3 m_LastValidLoc;
	AABB aabb;

	bool m_Sprinting;
	float m_Reach;

	Vector3i* breakIntersect();

	glm::vec3* placeIntersect();

	Vector3i sectionLocal(Vector3i worldPos);

	void updateMeshes(Vector3i editPos);

	void calculateVelocity();

public:
	Player(Camera& cam, ChunkManager* manager, float reach);

	void move();

	void breakBlock();

	void placeBlock(BlockType block);

	float getReach() const;

	void setReach(float reach);

	void setManager(ChunkManager* manager);

	glm::vec3 & getVelocity();

	bool isSprinting();
	
	void setSprinting(bool sprinting);
};

#endif