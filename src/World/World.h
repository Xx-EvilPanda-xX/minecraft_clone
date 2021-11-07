#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <unordered_map>
#include "Chunk.h"
#include "../Gen/TerrainGenerator.h"
#include "../Render/Renderer.h"
#include "../Render/Shader.h"
#include "ChunkManager.h"

class World
{
private:
	std::vector<Chunk*> m_Chunks;
	TerrainGenerator m_WorldGen;
	Shader m_Shader;
	ChunkManager m_Manager;
	int shouldGen;

public:
	World(TerrainGenerator worldGen, Shader shader);

	World();

	~World();

	void worldRender(Camera& camera);

	void generate();

	std::vector<Chunk*>& getChunks();
};

#endif
