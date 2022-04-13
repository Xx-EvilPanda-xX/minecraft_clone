#include "World.h"
#include <iostream>
#include <cmath>
#include <GLFW/glfw3.h>
#include "Chunk.h"
#include "../Math/Vector2i.h"
#include "../Render/Shader.h"
#include "ChunkManager.h"
#include "../Constants.h"

#define DEBUG
constexpr int chunkBuildsPerFrame{ constants::renderDistance / 2 };

World::World(Shader shader, Player player, ChunkManager& manager)
	: m_Shader{ shader },
	m_Manager{ manager },
	m_Player{ player },
	m_WorldGen{ manager }
{
}

void World::worldUpdate(const Camera& camera, bool deletePass)
{
	Vector3i playerPos{ getPlayerBlockPos(camera) };

	if (m_Manager.chunkExsists(playerPos))
		m_Player.move();

	m_Manager.updateQueues(camera);

	for (int i{}; i < chunkBuildsPerFrame; ++i)
	{
		genPass();
		buildPass();
	}

	if (deletePass)
		destroyPass(Vector2i{ playerPos.x, playerPos.z });

	std::vector<QueueBlock>& blockQueue{ m_WorldGen.getBlockQueue() };
	for (int i{}; i < blockQueue.size(); ++i)
	{
		QueueBlock queueBlock{ blockQueue.at(i) };
		if (m_Manager.chunkExsists(queueBlock.loc.worldPos))
		{
			m_Manager.getChunk(queueBlock.loc.worldPos)->getSection(queueBlock.loc.sectionIndex)->setBlock(queueBlock.sectionRelativePos, queueBlock.block.getType(), queueBlock.block.isSurface());
			blockQueue.erase(blockQueue.begin() + i);
			--i;
		}
	}
}

void World::worldRender(const Camera& camera, const Window& window)
{
	Vector3i playerPos{ getPlayerBlockPos(camera) };
	m_Shader.bind();
	m_Shader.setBool("playerUnderWater", m_Manager.getWorldBlock(playerPos).getType() == BlockType::Water);
	m_Shader.unbind();

	for (int i{}; i < m_Chunks.size(); ++i)
	{
		if (m_Chunks[i]->isBuilt())
			Renderer::drawMesh(camera, m_Chunks[i]->getMesh(), window);
	}
}

void World::genPass()
{
	if (!m_Manager.getGenQueue().empty())
	{
		Vector2i chunkPos{ m_Manager.getGenQueue().back() };
		m_Manager.getGenQueue().pop_back();

#ifdef DEBUG
		if (chunkPos.x > 4 || chunkPos.x < -4 || chunkPos.y > 4 || chunkPos.y < -4)
			return;
#endif // DEBUG


		Chunk* chunk{ new Chunk(chunkPos, m_Shader) };

		int** heightMap{ m_WorldGen.getHeightMap(chunk) };
		for (int i{}; i < g_ChunkCap; ++i)
		{
			SectionLocation section{ i, chunkPos };
			chunk->addSection(m_WorldGen.genSection(heightMap, section));
		}

		for (int i{}; i < 16; ++i)
		{
			delete[] heightMap[i];
		}

		delete[] heightMap;

		m_Chunks.push_back(chunk);
	}
}

void World::destroyPass(Vector2i playerPos)
{
	playerPos.x /= 16;
	playerPos.y /= 16;

	for (int i{}; i < m_Chunks.size(); ++i)
	{
		Vector2i chunkLoc{ m_Chunks[i]->getLocation() };

		//+ 1 becuase otherwise chunks that had just been generated got deleted
		if (std::abs(chunkLoc.x - playerPos.x) > constants::renderDistance + 1 || std::abs(chunkLoc.y - playerPos.y) > constants::renderDistance + 1)
		{
			int index;
			if (m_Manager.isInBuildQueue(m_Chunks[i], index))
				m_Manager.getBuildQueue().erase(m_Manager.getBuildQueue().begin() + index);

			if (m_Manager.isInBuildQueue(m_Manager.getChunk(Vector2i{ chunkLoc.x + 1, chunkLoc.y }), index))
				m_Manager.getBuildQueue().erase(m_Manager.getBuildQueue().begin() + index);

			if (m_Manager.isInBuildQueue(m_Manager.getChunk(Vector2i{ chunkLoc.x, chunkLoc.y + 1 }), index))
				m_Manager.getBuildQueue().erase(m_Manager.getBuildQueue().begin() + index);

			if (m_Manager.isInBuildQueue(m_Manager.getChunk(Vector2i{ chunkLoc.x - 1, chunkLoc.y }), index))
				m_Manager.getBuildQueue().erase(m_Manager.getBuildQueue().begin() + index);

			if (m_Manager.isInBuildQueue(m_Manager.getChunk(Vector2i{ chunkLoc.x, chunkLoc.y - 1 }), index))
				m_Manager.getBuildQueue().erase(m_Manager.getBuildQueue().begin() + index);

			delete m_Chunks[i];
			m_Chunks.erase(m_Chunks.begin() + i);

			//erasing an element from the middle of the vec shifts all elements beyond that one down once, essentially incrementing i
			--i;
		}
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
		Chunk* adjacentChunks[4];
		m_Manager.getAdjacentChunks(currentChunk->getLocation(), adjacentChunks);
		currentChunk->buildMesh(m_Manager, sectionPtr, adjacentChunks);
		++sectionPtr;

		if (sectionPtr == g_ChunkCap)
		{
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

int World::getChunkIndex(Vector2i chunkPos) const
{
	for (int i{}; i < m_Chunks.size(); ++i)
	{
		if (m_Chunks[i]->getLocation() == chunkPos)
			return i;
	}

	return -1;
}

Vector3i World::getPlayerBlockPos(const Camera& camera)
{
	Vector3i playerPos{ camera.getLocation() };

	if (camera.getLocation().x < 0.0)
		--playerPos.x;
	if (camera.getLocation().y < 0.0)
		--playerPos.y;
	if (camera.getLocation().z < 0.0)
		--playerPos.z;

	return playerPos;
}

std::vector<Chunk*>& World::getChunks()
{
	return m_Chunks;
}

ChunkManager& World::getManager()
{
	return m_Manager;
}

Player& World::getPlayer()
{
	return m_Player;
}