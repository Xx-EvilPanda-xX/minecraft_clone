#include "World.h"
#include "Chunk.h"
#include "../Math/Vector2i.h"
#include "../Render/Shader.h"
#include "ChunkManager.h"

static constexpr int g_WorldWidth{ 10 };
static constexpr int g_WorldHeight{ 10 };

World::World(TerrainGenerator worldGen, Shader shader) : m_WorldGen{ worldGen }, m_Shader{ shader }, m_Manager{ this }
{
}

World::~World()
{
}

void World::generate()
{
	for (int i{}; i < g_WorldWidth; ++i)
	{
		for (int j{}; i < g_WorldHeight; ++i)
		{
			Chunk* chunk = new Chunk(Vector2i{ i, j });

			for (int k{}; k < g_ChunkCap; ++k)
			{
				chunk->addSection(m_WorldGen.genSection(Vector2i{ i, j }, k));
			}

			m_Chunks.push_back(chunk);
		}
	}
}

void World::worldRender()
{
	static int meshPtr{};

	if (meshPtr < m_Chunks.size())
	{
		m_Chunks[meshPtr]->buildMesh(m_Manager);
		++meshPtr;
	}
	
	for (int i{}; i < m_Chunks.size(); ++i)
	{
		Renderer::drawMesh(m_Shader, m_Chunks[i]->getMesh());
	}
}

std::vector<Chunk*> World::getChunks()
{
	return m_Chunks;
}