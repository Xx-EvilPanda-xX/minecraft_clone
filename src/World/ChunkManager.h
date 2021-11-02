#ifndef CHUNK_MANAGER_H
#define CHUNK_MANAGER_H

#include "Block.h"
#include <glm/vec3.hpp>

class World;

class ChunkManager
{
private:
	World* m_World;

public:
	ChunkManager(World* world);

	~ChunkManager();

	void setWorldBlock(glm::vec3 loc, Block block);

	Block getWorldBlock(glm::vec3 loc);
};

#endif