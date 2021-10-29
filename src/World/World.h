#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include "Chunk.h"
#include "../Gen/TerrainGenerator.h"
#include "../Render/Renderer.h"
#include "../Render/Shader.h"

class World
{
private:
	std::vector<Chunk*> m_Chunks;
	TerrainGenerator m_WorldGen;
	Shader m_Shader;

public:
	World(TerrainGenerator worldGen);

	~World();

	void worldRender();

	void generate();

	void updateChunk(Chunk* chunk);
};

#endif
