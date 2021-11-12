#include "World.h"
#include <iostream>
#include <GLFW/glfw3.h>
#include "Chunk.h"
#include "../Math/Vector2i.h"
#include "../Render/Shader.h"
#include "ChunkManager.h"

static constexpr int g_WorldWidth{ 16 };
static constexpr int g_WorldHeight{ 16 };

World::World(TerrainGenerator worldGen, Shader shader) : m_WorldGen{ worldGen }, m_Shader{ shader }, m_Manager{ this }
{
}

World::World() = default;

bool pumiscoot = true;

void World::worldRender(Camera& camera)
{
	static int x{ -16 };
	static int y{ -16 };
	static bool isGenerating{ true };
	static int meshPtr{};
	static int sectionPtr{ 0 };
	static constexpr int genInterval{ 1 };
	static double start{};

	if (sectionPtr == 0)
	{
		start = glfwGetTime();

		if (isGenerating)
		{
			Chunk* chunk{ new Chunk(Vector2i{ x, y }, m_Shader) };

			int** heightMap{ m_WorldGen.getHeightMap() };
			for (int k{}; k < g_ChunkCap; ++k)
			{
				chunk->addSection(m_WorldGen.genSection(heightMap, k));
			}

			for (int i{}; i < 16; ++i)
			{
				delete[] heightMap[i];
			}

			delete[] heightMap;

			m_Chunks.push_back(chunk);

			++x;

			if (x == 0)
			{
				x = -16;
				++y;
			}

			if (y == 0)
				isGenerating = false;
		}
	}

	if (meshPtr < m_Chunks.size() && shouldGen >= genInterval)
	{
		m_Chunks[meshPtr]->buildMesh(m_Manager);
		++sectionPtr;

		if (sectionPtr == g_ChunkCap)
		{
			std::cout << "Mesh built at " << m_Chunks[meshPtr]->getLocation().x << ", " << m_Chunks[meshPtr]->getLocation().y << " in " << static_cast<long>((glfwGetTime() - start) * 1000) << "ms\n";
			
			++meshPtr;
			sectionPtr = 0;
		}

		shouldGen = 0;
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