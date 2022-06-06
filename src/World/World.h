#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <mutex>
#include <unordered_map>
#include "Chunk.h"
#include "../Gen/TerrainGenerator.h"
#include "../Render/Renderer.h"
#include "../Render/Shader.h"
#include "ChunkManager.h"
#include "../Player/Player.h"

constexpr int genInterval{ 0 };

class World
{
private:
	std::mutex m_ChunksMutex;
	std::vector<Chunk*> m_Chunks;

	std::mutex m_UploadPendingMutex;
	std::vector<Chunk*> m_UploadPending;

	TerrainGenerator m_WorldGen;
	Shader m_Shader;
	ChunkManager m_Manager;
	Player m_Player;

	Vector2i m_LastPlayerChunkPos;
	int m_LastBlockQueueSize;
	int m_MoveCountDown;
	bool m_ResetBuildVars;

	void uploadAll();

public:
	World(Shader shader, Keyboard& keyboard);

	void worldRender(const Window& window);
	
	void worldUpdate();

	void genPass();

	void destroyPass(Vector2i playerPos);

	void buildPass();

	void rebuildChunks(const Camera& camera);

	int getChunkIndex(Vector2i chunkPos);

	std::mutex& getChunksMutex();

	std::vector<Chunk*>& getChunks();

	std::vector<Chunk*>& getUploadPending();

	ChunkManager& getManager();

	Player& getPlayer();

	void placeQueueBlocks();
};

#endif
