#include "World.h"
#include <iostream>
#include <GLFW/glfw3.h>
#include "Chunk.h"
#include "../Math/Vector2i.h"
#include "../Render/Shader.h"
#include "ChunkManager.h"

World::World(TerrainGenerator worldGen, Shader shader) : m_WorldGen{ worldGen }, m_Shader{ shader }, m_Manager{ this }
{
}

World::World() = default;

void World::worldRender(const Camera& camera, bool updateQueues)
{
	static int sectionPtr{ 0 };

	if (sectionPtr == 0)
	{
		genPass();
	}

	if (updateQueues)
	{
		m_Manager.updateQueues(camera);
	}

	buildPass(sectionPtr);
	
	for (int i{}; i < m_Chunks.size(); ++i)
	{
		Renderer::drawMesh(camera, m_Chunks[i]->getMesh());
	}
}

void World::genPass()
{
	if (!m_Manager.getGenQueue().empty())
	{
		Vector2i chunkPos{ m_Manager.getGenQueue().back() };
		m_Manager.getGenQueue().pop_back();

		Chunk* chunk{ new Chunk(chunkPos, m_Shader) };

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
		std::cout << "Chunk generated at : " << m_Chunks[m_Chunks.size() - 1]->getLocation().x << ", " << m_Chunks[m_Chunks.size() - 1]->getLocation().y << "\n";
	}
}

void World::buildPass(int& sectionPtr)
{
	static int meshPtr{};
	static constexpr int genInterval{ 1 };
	static Chunk* currentChunk{ nullptr };

	if (!m_Manager.getBuildQueue().empty() && sectionPtr == 0)
		currentChunk = m_Manager.getBuildQueue().back();

	if (currentChunk && shouldGen >= genInterval)
	{
		currentChunk->buildMesh(m_Manager);
		++sectionPtr;

		if (sectionPtr == g_ChunkCap)
		{
			std::cout << "Mesh built at " << currentChunk->getLocation().x << ", " << currentChunk->getLocation().y << "\n";
			m_Manager.getBuildQueue().pop_back();
			sectionPtr = 0;
		}

		shouldGen = 0;
	}
	++shouldGen;
}

std::vector<Chunk*>& World::getChunks()
{
	return m_Chunks;
}