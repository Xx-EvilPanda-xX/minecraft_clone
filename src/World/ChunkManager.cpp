#include <cmath>
#include <chrono>
#include "ChunkManager.h"
#include "../Constants.h"
#include "Block.h"
#include "../Math/Vector3i.h"
#include "../Math/Vector2i.h"
#include "World.h"

ChunkManager::ChunkManager(World* world) : m_World{ world }
{
}

ChunkManager::ChunkManager() = default;

void ChunkManager::setWorldBlock(Vector3i loc, Block block)
{
	if (loc.y < 0 || loc.y > 255)
		return;

	int chunkIndex{};
	int sectionIndex{};

	Vector3i sectionLocal{ loc.x < 0 ? 16 + (loc.x % 16) : loc.x % 16, loc.y < 0 ? 16 + (loc.y % 16) : loc.y % 16, loc.z < 0 ? 16 + (loc.z % 16) : loc.z % 16 };
	Vector2i chunkLocation{ loc.x / 16, loc.z / 16 };

	if (loc.x < 0)
		--chunkLocation.x;

	if (loc.z < 0)
		--chunkLocation.y;

	bool foundChunk{ false };

	for (int i{}; i < m_World->getChunks().size(); ++i)
	{
		if (m_World->getChunks()[i]->getLocation() == chunkLocation)
		{
			chunkIndex = i;
			foundChunk = true;
			break;
		}
	}

	if (!foundChunk)
		return;

	sectionIndex = loc.y / 16;

	if (sectionIndex > 15)
		return;

	m_World->getChunks()[chunkIndex]->getSection(sectionIndex)->setBlock(sectionLocal, block);
}

Block ChunkManager::getWorldBlock(Vector3i loc)
{
	if (loc.y < 0 || loc.y > 255)
		return Block{};

	int chunkIndex{};
	int sectionIndex{};

	Vector3i sectionLocal{loc.x % 16, loc.y % 16, loc.z % 16 };
	if (loc.x < 0 && sectionLocal.x != 0)
		sectionLocal.x += 16;
	if (loc.y < 0 && sectionLocal.y != 0)
		sectionLocal.y += 16;
	if (loc.z < 0 && sectionLocal.z != 0)
		sectionLocal.z += 16;

	Vector2i chunkLocation{ loc.x / 16, loc.z / 16 };

	if (loc.x < 0 && sectionLocal.x != 0)
		--chunkLocation.x;

	if (loc.z < 0 && sectionLocal.z != 0)
		--chunkLocation.y;

	bool foundChunk{ false };

	for (int i{}; i < m_World->getChunks().size(); ++i)
	{
		if (m_World->getChunks()[i]->getLocation() == chunkLocation)
		{
			chunkIndex = i;
			foundChunk = true;
			break;
		}
	}

	if (!foundChunk)
		return Block{};

	sectionIndex = loc.y / 16;

	if (sectionIndex > 15)
		return Block{};

	return m_World->getChunks()[chunkIndex]->getSection(sectionIndex)->getBlock(sectionLocal);
}

bool ChunkManager::chunkExsists(Vector3i loc)
{
	Vector2i chunkLocation{ loc.x / 16, loc.z / 16 };

	if (loc.x < 0)
		--chunkLocation.x;

	if (loc.z < 0)
		--chunkLocation.y;

	for (int i{}; i < m_World->getChunks().size(); ++i)
	{
		if (m_World->getChunks()[i]->getLocation() == chunkLocation)
		{
			return true;
		}
	}

	return false;
}

bool ChunkManager::chunkExsists(Vector2i chunkLoc)
{
	for (int i{}; i < m_World->getChunks().size(); ++i)
	{
		if (m_World->getChunks()[i]->getLocation() == chunkLoc)
		{
			return true;
		}
	}

	return false;
}

void ChunkManager::updateGenQueue(const Camera& player)
{
	glm::vec3 pos{ player.getLocation() };
	Vector3i playerPos{ static_cast<int>(pos.x), static_cast<int>(pos.y), static_cast<int>(pos.z) };
	Vector2i chunkPlayerPos{ playerPos.x / 16, playerPos.z / 16 };
	
	for (int x{ chunkPlayerPos.x - constants::renderDistance }; x < chunkPlayerPos.x + constants::renderDistance; ++x)
	{
		for (int y{ chunkPlayerPos.y - constants::renderDistance };  y < chunkPlayerPos.y + constants::renderDistance; ++y)
		{
			Vector2i chunkPos{ x, y };
			if (!chunkExsists(chunkPos) && !isInGenQueue(chunkPos))
				m_GenQueue.insert(m_GenQueue.begin(), chunkPos);
		}
	}
}

void ChunkManager::updateBuildQueue()
{
	for (int i{}; i < m_BuildQueue.size(); ++i)
	{
		if (!chunkExsists(m_BuildQueue[i]->getLocation()))
		{
			m_BuildQueue.erase(m_BuildQueue.begin() + i);
		}
	}

	for (int i{}; i < m_World->getChunks().size(); ++i)
	{
		if (m_World->getChunks()[i]->isBuilt() || isInBuildQueue(m_World->getChunks()[i]))
			continue;

		Vector2i chunkLoc{ m_World->getChunks()[i]->getLocation() };
		if (chunkExsists(Vector2i{ chunkLoc.x + 1, chunkLoc.y }))
		{
			if (chunkExsists(Vector2i{ chunkLoc.x - 1, chunkLoc.y }))
			{
				if (chunkExsists(Vector2i{ chunkLoc.x, chunkLoc.y + 1}))
				{
					if (chunkExsists(Vector2i{ chunkLoc.x, chunkLoc.y - 1 }))
					{
						m_BuildQueue.insert(m_BuildQueue.begin(), m_World->getChunks()[i]);
					}
				}
			}
		}
	}
}

void ChunkManager::updateQueues(const Camera& player)
{
	updateGenQueue(player);
	updateBuildQueue();
}

bool ChunkManager::isInGenQueue(Vector2i gen)
{
	for (int i{}; i < m_GenQueue.size(); ++i)
	{
		if (m_GenQueue[i] == gen)
			return true;
	}

	return false;
}

bool ChunkManager::isInBuildQueue(Chunk* build)
{
	for (int i{}; i < m_BuildQueue.size(); ++i)
	{
		if (m_BuildQueue[i] == build)
			return true;
	}

	return false;
}

std::vector<Vector2i>& ChunkManager::getGenQueue()
{
	return m_GenQueue;
}

std::vector<Chunk*>& ChunkManager::getBuildQueue()
{
	return m_BuildQueue;
}

