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

constexpr int genInterval{ 2 };

class World
{
private:
	std::vector<Chunk*> m_Chunks;
	TerrainGenerator m_WorldGen;
	Shader m_Shader;
	ChunkManager m_Manager;
	Player m_Player;

public:
	World(TerrainGenerator worldGen, Shader shader, Player player);

	void worldRender(const Camera& camera, bool moved);

	void genPass();

	void destroyPass(Vector2i playerPos);

	void buildPass();

	void reloadChunks(const Camera& camera);

	int getChunkIndex(Vector2i chunkPos) const;

	std::vector<Chunk*>& getChunks();

	ChunkManager& getManager();

	Player& getPlayer();
};

#endif
