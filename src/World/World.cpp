#include "World.h"
#include <iostream>
#include "Chunk.h"
#include "../Math/Vector2i.h"
#include "../Render/Shader.h"
#include "ChunkManager.h"

static constexpr int g_WorldWidth{ 5 };
static constexpr int g_WorldHeight{ 5 };

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
			Chunk* chunk = new Chunk(Vector2i{ i * 16, j * 16 }, m_Shader);

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

	if (meshPtr < m_Chunks.size())
	{
		m_Chunks[meshPtr]->buildMesh(m_Manager);
		++meshPtr;
		std::cout << "mesh built\n";
	}
	
	for (int i{}; i < m_Chunks.size(); ++i)
	{
		Renderer::drawMesh(camera, m_Chunks[i]->getMesh());
	}
}

std::vector<Chunk*>& World::getChunks()
{
	return m_Chunks;
}