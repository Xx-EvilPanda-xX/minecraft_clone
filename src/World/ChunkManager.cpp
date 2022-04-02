#include <cmath>
#include <chrono>
#include "ChunkManager.h"
#include "../Constants.h"
#include "Block.h"
#include "../Math/Vector3i.h"
#include "../Math/Vector2i.h"
#include "World.h"

ChunkManager::ChunkManager(World* world) 
	: m_World{ world }
{
}

ChunkManager::ChunkManager() = default;

void ChunkManager::setWorldBlock(Vector3i loc, BlockType type, bool surface)
{
	Vector2i chunkPos{};
	int sectionIndex{};
	Vector3i sectionLocal{};

	if (getBlockAbsoluteIndex(loc, chunkPos, sectionIndex, sectionLocal))
		m_World->getChunks()[chunkPos]->getSection(sectionIndex)->setBlock(sectionLocal, type, surface);
}

Block ChunkManager::getWorldBlock(Vector3i loc)
{
	Vector2i chunkPos{};
	int sectionIndex{};
	Vector3i sectionLocal{};

	if (getBlockAbsoluteIndex(loc, chunkPos, sectionIndex, sectionLocal))
		return m_World->getChunks()[chunkPos]->getSection(sectionIndex)->getBlock(sectionLocal);
	else
		return Block{};
}

bool ChunkManager::getBlockAbsoluteIndex(Vector3i loc, Vector2i& o_ChunkPos, int& o_SectionIndex, Vector3i& o_SectionLocal)
{
	bool success{ true };

	if (loc.y < 0 || loc.y > 255)
		success = false; 

	int sectionIndex{};

	Vector3i sectionLocal{ toSectionLocal(loc) };
	Vector2i chunkLocation{ loc.x / 16, loc.z / 16 };

	if (loc.x < 0 && sectionLocal.x != 0)
		--chunkLocation.x;

	if (loc.z < 0 && sectionLocal.z != 0)
		--chunkLocation.y;

	if (m_World->getChunks().find(chunkLocation) == m_World->getChunks().end())
		success = false;

	sectionIndex = loc.y / 16;

	if (sectionIndex > 15)
		success = false;

	if (success)
	{
		o_ChunkPos = chunkLocation;
		o_SectionIndex = sectionIndex;
		o_SectionLocal = sectionLocal;
	}

	return success;
}

bool ChunkManager::chunkExsists(Vector3i loc) const
{
	if (getChunk(loc))
		return true;
	else
		return false;
}

bool ChunkManager::chunkExsists(Vector2i chunkLoc) const
{
	if (getChunk(chunkLoc))
		return true;
	else
		return false;
}

void ChunkManager::updateGenQueue(const Camera& player)
{
	glm::dvec3 pos{ player.getLocation() };
	Vector3i playerPos{ static_cast<int>(pos.x), static_cast<int>(pos.y), static_cast<int>(pos.z) };
	Vector2i chunkPlayerPos{ playerPos.x / 16, playerPos.z / 16 };
	
	for (int i{}; i <= constants::renderDistance; ++i)
	{
		for (int x{ chunkPlayerPos.x - i }; x <= chunkPlayerPos.x + i; ++x)
		{
			for (int y{ chunkPlayerPos.y - i }; y <= chunkPlayerPos.y + i; ++y)
			{
				if (std::abs(chunkPlayerPos.x - x) == i || std::abs(chunkPlayerPos.y - y) == i)
				{
					Vector2i chunkPos{ x, y };
					if (!chunkExsists(chunkPos) && !isInGenQueue(chunkPos))
						m_GenQueue.insert(m_GenQueue.begin(), chunkPos);
				}
			}
		}
	}
}

void ChunkManager::updateBuildQueue()
{
	for (std::pair<Vector2i, Chunk*> p : m_World->getChunks())
	{
		Chunk* chunk{ p.second };
		if (chunk->isBuilt() || isInBuildQueue(chunk))
			continue;

		Vector2i chunkLoc{ chunk->getLocation() };
		if (chunkExsists(Vector2i{ chunkLoc.x + 1, chunkLoc.y }))
		{
			if (chunkExsists(Vector2i{ chunkLoc.x - 1, chunkLoc.y }))
			{
				if (chunkExsists(Vector2i{ chunkLoc.x, chunkLoc.y + 1}))
				{
					if (chunkExsists(Vector2i{ chunkLoc.x, chunkLoc.y - 1 }))
					{
						m_BuildQueue.insert(m_BuildQueue.begin(), chunk);
					}
				}
			}
		}
	}
}

void ChunkManager::updateQueues(const Camera& player)
{
	if (m_GenQueue.empty())
		updateGenQueue(player);

	if (m_BuildQueue.empty())
		updateBuildQueue();
}

bool ChunkManager::isInGenQueue(Vector2i gen, int& o_Index)
{
	for (int i{}; i < m_GenQueue.size(); ++i)
	{
		if (m_GenQueue[i] == gen)
		{
			o_Index = i;
			return true;
		}
	}

	return false;
}

bool ChunkManager::isInBuildQueue(Chunk* build, int& o_Index)
{
	for (int i{}; i < m_BuildQueue.size(); ++i)
	{
		if (m_BuildQueue[i] == build)
		{
			o_Index = i;
			return true;
		}
	}

	return false;
}

Vector3i ChunkManager::toSectionLocal(Vector3i worldPos)
{
	Vector3i sectionLocal{ worldPos.x % 16, worldPos.y % 16, worldPos.z % 16 };
	if (worldPos.x < 0 && sectionLocal.x != 0)
		sectionLocal.x += 16;
	if (worldPos.y < 0 && sectionLocal.y != 0)
		sectionLocal.y += 16;
	if (worldPos.z < 0 && sectionLocal.z != 0)
		sectionLocal.z += 16;

	return sectionLocal;
}

bool ChunkManager::isInGenQueue(Vector2i gen)
{
	int index;

	return isInGenQueue(gen, index);
}

bool ChunkManager::isInBuildQueue(Chunk* build)
{
	int index;

	return isInBuildQueue(build, index);
}

std::vector<Vector2i>& ChunkManager::getGenQueue()
{
	return m_GenQueue;
}

std::vector<Chunk*>& ChunkManager::getBuildQueue()
{
	return m_BuildQueue;
}

Chunk* ChunkManager::getChunk(Vector3i loc) const
{
	Vector2i chunkLocation{ loc.x / 16, loc.z / 16 };

	if (loc.x < 0 && loc.x % 16 != 0)
		--chunkLocation.x;

	if (loc.z < 0 && loc.z % 16 != 0)
		--chunkLocation.y;

	return getChunk(chunkLocation);
}

Chunk* ChunkManager::getChunk(Vector2i chunkLoc) const
{
	if (m_World->getChunks().find(chunkLoc) != m_World->getChunks().end())
		return m_World->getChunks().at(chunkLoc);

	return nullptr;
}

void ChunkManager::getAdjacentChunks(Vector2i chunkLoc, Chunk* o_AdjacentChunks[4]) const
{
	o_AdjacentChunks[0] = getChunk(Vector2i{ chunkLoc.x + 1, chunkLoc.y });
	o_AdjacentChunks[1] = getChunk(Vector2i{ chunkLoc.x, chunkLoc.y + 1 });
	o_AdjacentChunks[2] = getChunk(Vector2i{ chunkLoc.x - 1, chunkLoc.y });
	o_AdjacentChunks[3] = getChunk(Vector2i{ chunkLoc.x, chunkLoc.y - 1 });
}

void ChunkManager::clearQueues()
{
	m_GenQueue.clear();
	m_BuildQueue.clear();
}

void ChunkManager::setWorld(World* world)
{
	m_World = world;
}

