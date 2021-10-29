#include "World.h"
#include "Chunk.h"
#include "../Math/Vector2i.h"
#include "../Render/Shader.h"

static constexpr int g_WorldWidth{ 10 };
static constexpr int g_WorldHeight{ 10 };

World::World(TerrainGenerator worldGen) : m_WorldGen{ worldGen }, m_Shader{ Shader{ "assets/shaders/vert.glsl", "assets/shaders/frag.glsl" } }
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
			Chunk* chunk = new Chunk();

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
	for (int i{}; i < m_Chunks.size(); ++i)
	{
		updateChunk(m_Chunks[i]);
		Renderer::drawMesh(m_Shader, m_Chunks[i]->getMesh());
	}
}

void World::updateChunk(Chunk* chunk)
{
	chunk->buildMesh();
}