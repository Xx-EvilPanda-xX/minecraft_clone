#ifndef CHUNK_MANAGER_H
#define CHUNK_MANAGER_H

#include <vector>
#include "Block.h"
#include "../Player/Camera.h"
#include "../Math/Vector3i.h"
#include "../Math/Vector2i.h"

class World;
class Chunk;

class ChunkManager
{
private:
	World* m_World;
	std::vector<Vector2i> m_GenQueue;
	std::vector<Chunk*> m_BuildQueue;

	void updateGenQueue(const Camera& player);

	void updateBuildQueue();

	bool isInGenQueue(Vector2i gen);

	bool isInBuildQueue(Chunk* build);

public:
	ChunkManager(World* world);

	ChunkManager();

	void setWorldBlock(Vector3i loc, Block block);

	Block getWorldBlock(Vector3i loc);

	bool chunkExsists(Vector3i loc);

	bool chunkExsists(Vector2i chunkLoc);

	void updateQueues(const Camera& player);

	std::vector<Vector2i>& getGenQueue();

	std::vector<Chunk*>& getBuildQueue();

	void clearQueues();
};

#endif