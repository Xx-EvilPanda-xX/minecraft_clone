#ifndef CHUNK_MANAGER_H
#define CHUNK_MANAGER_H

#include "Block.h"
#include "../Math/Vector3i.h"

class World;

class ChunkManager
{
private:
	World* m_World;

public:
	ChunkManager(World* world);

	~ChunkManager();

	void setWorldBlock(Vector3i loc, Block block);

	Block getWorldBlock(Vector3i loc);
};

#endif