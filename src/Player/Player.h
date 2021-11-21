#ifndef PLAYER_H
#define PLAYER_H

#include <glm/glm.hpp>
#include "Camera.h"
#include "../World/ChunkManager.h"

class Player
{
private:
	const Camera& m_Cam;
	ChunkManager* m_Manager;

	float m_Reach;

	Vector3i* breakIntersect();

	glm::vec3* placeIntersect();

	Vector3i sectionLocal(Vector3i worldPos);

	void updateMeshes(Vector3i editPos);

public:
	Player(const Camera& cam, ChunkManager* manager, float reach);

	void breakBlock();

	void placeBlock(Block block);

	float getReach() const;

	void setReach(float reach);

	void setManager(ChunkManager* manager);
};

#endif