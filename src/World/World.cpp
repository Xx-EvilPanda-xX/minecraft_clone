#include "World.h"
#include <iostream>
#include "Chunk.h"
#include "../Math/Vector2i.h"
#include "../Render/Shader.h"
#include "ChunkManager.h"

static constexpr int g_WorldWidth{ 20 };
static constexpr int g_WorldHeight{ 20 };

World::World(TerrainGenerator worldGen, Shader shader) : m_WorldGen{ worldGen }, m_Shader{ shader }, m_Manager{ this }
{
}

World::World() = default;

World::~World()
{
}

void World::generate()
{
	for (int i{}; i < g_WorldWidth; ++i)
	{
		for (int j{}; j < g_WorldHeight; ++j)
		{
			Chunk* chunk = new Chunk(Vector2i{ i, j }, m_Shader);

			for (int k{}; k < g_ChunkCap; ++k)
			{
				chunk->addSection(m_WorldGen.genSection(k));
			}

			m_Chunks.push_back(chunk);
		}
	}
}

void World::worldRender(Camera& camera)
{
	static int meshPtr{};
	static constexpr int genInterval{ 1 };

	if (meshPtr < m_Chunks.size() && shouldGen == genInterval)
	{
		m_Chunks[meshPtr]->buildMesh(m_Manager);
		++meshPtr;
		shouldGen = 0;
		std::cout << "mesh built\n";
	}
	++shouldGen;
	
	for (int i{}; i < m_Chunks.size(); ++i)
	{
		Renderer::drawMesh(camera, m_Chunks[i]->getMesh());
	}
}

std::vector<Chunk*>& World::getChunks()
{
	return m_Chunks;
}