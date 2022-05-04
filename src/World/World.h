#ifndef WORLD_H
#define WORLD_H

#include <vector>
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
	std::vector<Chunk*> m_Chunks;
	TerrainGenerator m_WorldGen;
	Shader m_Shader;
	ChunkManager m_Manager;
	Player m_Player;

	int m_LastBlockQueueSize;
	bool m_ResetBuildVars;

public:
	World(Shader shader, Keyboard& keyboard);

	void worldRender(const Window& window);
	
	void worldUpdate(bool deletePass);

	void genPass();

	void destroyPass(Vector2i playerPos);

	void buildPass();

	void rebuildChunks(const Camera& camera);

	int getChunkIndex(Vector2i chunkPos) const;

	std::vector<Chunk*>& getChunks();

	ChunkManager& getManager();

	Player& getPlayer();
};

#endif
