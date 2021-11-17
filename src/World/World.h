#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <unordered_map>
#include "Chunk.h"
#include "../Gen/TerrainGenerator.h"
#include "../Render/Renderer.h"
#include "../Render/Shader.h"
#include "ChunkManager.h"

constexpr int genInterval{ 2 };

class World
{
private:
	std::vector<Chunk*> m_Chunks;
	TerrainGenerator m_WorldGen;
	Shader m_Shader;
	ChunkManager m_Manager;

public:
	World(TerrainGenerator worldGen, Shader shader);

	World();

	void worldRender(const Camera& camera, bool moved);

	void genPass();

	void destroyPass(Vector2i playerPos);

	void buildPass();

	void reloadChunks(const Camera& camera);

	int getChunkIndex(Vector2i chunkPos);

	std::vector<Chunk*>& getChunks();

	ChunkManager& getManager();
};

#endif
