#ifndef CHUNK_MANAGER_H
#define CHUNK_MANAGER_H

#include <vector>
#include <mutex>
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
	std::mutex m_GenQueueMutex;
	std::vector<Vector2i> m_GenQueue;
	std::mutex m_BuildQueueMutex;
	std::vector<Chunk*> m_BuildQueue;

	void updateGenQueue(const Camera& player);

	void updateBuildQueue();

	bool getBlockAbsoluteIndex(Vector3i loc, 
							int& o_ChunkIndex,
							int& o_SectionIndex,
							Vector3i& o_SectionLocal);

public:
	ChunkManager(World* world);

	ChunkManager();

	void setWorldBlock(Vector3i loc, BlockType type, bool surface, bool wasPlayer);

	Block getWorldBlock(Vector3i loc);

	bool chunkExsists(Vector3i loc) const;

	bool chunkExsists(Vector2i chunkLoc) const;

	bool isInGenQueue(Vector2i gen);

	bool isInBuildQueue(Chunk* build);

	bool isInGenQueue(Vector2i gen, int& o_Index);

	bool isInBuildQueue(Chunk* build, int& o_Index);

	void updateQueues(const Camera& player);

	Vector3i toSectionLocal(Vector3i worldPos);

	std::mutex& getGenQueueMutex();

	std::vector<Vector2i>& getGenQueue();

	std::mutex& getBuildQueueMutex();

	std::vector<Chunk*>& getBuildQueue();

	Chunk* getChunk(Vector3i loc) const;

	Chunk* getChunk(Vector2i chunkLoc) const;

	void getAdjacentChunks(Vector2i chunkLoc, Chunk* o_AdjacentChunks[4]) const;

	void pushUploadPending(Chunk* chunk);

	void clearQueues();

	void setWorld(World* world);
};

#endif