#include "World.h"
#include <iostream>
#include <cmath>
#include <GLFW/glfw3.h>
#include "Chunk.h"
#include "../Math/Vector2i.h"
#include "../Render/Shader.h"
#include "ChunkManager.h"
#include "../Constants.h"

World::World(TerrainGenerator worldGen, Shader shader) : m_WorldGen{ worldGen }, m_Shader{ shader }, m_Manager{ this }
{
}

World::World() = default;

void World::worldRender(const Camera& camera, bool updateQueues)
{
	genPass();
	destroyPass(Vector2i{ static_cast<int>(camera.getLocation().x), static_cast<int>(camera.getLocation().z) });
		
	m_Manager.updateQueues(camera);

	buildPass();
	
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

		int** heightMap{ m_WorldGen.getHeightMap(chunk) };
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
		//std::cout << "Chunk generated at : " << m_Chunks[m_Chunks.size() - 1]->getLocation().x << ", " << m_Chunks[m_Chunks.size() - 1]->getLocation().y << "\n";
	}
}

void World::destroyPass(Vector2i playerPos)
{
	playerPos.x /= 16;
	playerPos.y /= 16;

	for (int i{}; i < m_Chunks.size(); ++i)
	{
		Vector2i chunkLoc{ m_Chunks[i]->getLocation() };

		if (std::abs(chunkLoc.x - playerPos.x) > constants::renderDistance || std::abs(chunkLoc.y - playerPos.y) > constants::renderDistance)
		{
			//std::cout << "Chunk deleted at: " << m_Chunks[i]->getLocation().x << ", " << m_Chunks[i]->getLocation().y << "\n";
			delete m_Chunks[i];
			m_Chunks[i] = nullptr;
		}
	}

	for (int i{ static_cast<int>(m_Chunks.size()) - 1 }; i >= 0; --i)
	{
		if (m_Chunks[i] == nullptr)
			m_Chunks.erase(m_Chunks.begin() + i);
	}
}

void World::buildPass()
{
	static int sectionPtr{};
	static int shouldGen{};

	Chunk* currentChunk{ nullptr };

	if (!m_Manager.getBuildQueue().empty())
		currentChunk = m_Manager.getBuildQueue().back();

	if (currentChunk != nullptr && shouldGen >= genInterval)
	{
		currentChunk->buildMesh(m_Manager, sectionPtr);
		++sectionPtr;

		if (sectionPtr == g_ChunkCap)
		{
			//std::cout << "Mesh built at " << currentChunk->getLocation().x << ", " << currentChunk->getLocation().y << "\n";
			m_Manager.getBuildQueue().pop_back();

			currentChunk = nullptr;

			sectionPtr = 0;
		}

		shouldGen = 0;
	}
	++shouldGen;
}

void World::reloadChunks(const Camera& camera)
{
	for (int i{}; i < m_Chunks.size(); ++i)
	{
		delete m_Chunks[i];
		m_Chunks[i] = nullptr;
	}

	m_Chunks.clear();
	m_Manager.clearQueues();
	m_Manager.updateQueues(camera);
}

int World::getChunkIndex(Vector2i chunkPos)
{
	for (int i{}; i < m_Chunks.size(); ++i)
	{
		if (m_Chunks[i]->getLocation() == chunkPos)
			return i;
	}

	return -1;
}

std::vector<Chunk*>& World::getChunks()
{
	return m_Chunks;
}

ChunkManager& World::getManager()
{
	return m_Manager;
}