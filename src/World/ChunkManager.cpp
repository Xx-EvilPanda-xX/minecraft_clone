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
	int chunkIndex{};
	int sectionIndex{};
	Vector3i sectionLocal{};

	if (getBlockAbsoluteIndex(loc, chunkIndex, sectionIndex, sectionLocal))
	{
		Chunk* chunk{};

		{
			std::lock_guard<std::mutex> lock{ m_World->getChunksMutex() };
			chunk = m_World->getChunks()[chunkIndex];
		}

		chunk->getSection(sectionIndex)->setBlock(sectionLocal, type, surface);
	}
}

Block ChunkManager::getWorldBlock(Vector3i loc)
{
	int chunkIndex{};
	int sectionIndex{};
	Vector3i sectionLocal{};

	if (getBlockAbsoluteIndex(loc, chunkIndex, sectionIndex, sectionLocal))
	{
		Chunk* chunk{};

		{
			std::lock_guard<std::mutex> lock{ m_World->getChunksMutex() };
			chunk = m_World->getChunks()[chunkIndex];
		}

		return chunk->getSection(sectionIndex)->getBlock(sectionLocal);
	}	
	else
		return Block{};
}

bool ChunkManager::getBlockAbsoluteIndex(Vector3i loc, int& o_ChunkIndex, int& o_SectionIndex, Vector3i& o_SectionLocal)
{
	bool success{ true };

	if (loc.y < 0 || loc.y > 255)
		success = false; 

	int chunkIndex{};
	int sectionIndex{};

	Vector3i sectionLocal{ toSectionLocal(loc) };
	Vector2i chunkLocation{ loc.x / 16, loc.z / 16 };

	if (loc.x < 0 && sectionLocal.x != 0)
		--chunkLocation.x;

	if (loc.z < 0 && sectionLocal.z != 0)
		--chunkLocation.y;

	bool foundChunk{ false };

	{
		std::lock_guard<std::mutex> lock{ m_World->getChunksMutex() };

		for (int i{}; i < m_World->getChunks().size(); ++i)
		{
			if (m_World->getChunks()[i]->getLocation() == chunkLocation)
			{
				chunkIndex = i;
				foundChunk = true;
				break;
			}
		}
	}

	if (!foundChunk)
		success = false;

	sectionIndex = loc.y / 16;

	if (sectionIndex > 15)
		success = false;

	if (success)
	{
		o_ChunkIndex = chunkIndex;
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
	Vector3i playerPos{ player.getLocation() };
	Vector2i chunkPlayerPos{ playerPos.x / 16, playerPos.z / 16 };
	
	for (int i{}; i <= constants::loadDistance; ++i)
	{
		for (int x{ chunkPlayerPos.x - i }; x <= chunkPlayerPos.x + i; ++x)
		{
			for (int y{ chunkPlayerPos.y - i }; y <= chunkPlayerPos.y + i; ++y)
			{
				if (std::abs(chunkPlayerPos.x - x) == i || std::abs(chunkPlayerPos.y - y) == i)
				{
					Vector2i chunkPos{ x, y };
					if (!chunkExsists(chunkPos) && !isInGenQueue(chunkPos))
					{
						std::lock_guard<std::mutex> lock{ m_GenQueueMutex };
						m_GenQueue.insert(m_GenQueue.begin(), chunkPos);
					}
				}
			}
		}
	}
}

void ChunkManager::updateBuildQueue()
{
	m_World->disallowChunkDestruction();
	std::vector<Chunk*> copy{};

	{
		std::lock_guard<std::mutex> lock{ m_World->getChunksMutex() };
		copy = m_World->getChunks();
	}

	for (int i{}; i < copy.size(); ++i)
	{
		if (copy[i]->isBuilt() || isInBuildQueue(copy[i]))
			continue;

		Vector2i chunkLoc{ copy[i]->getLocation() };
		if (chunkExsists(Vector2i{ chunkLoc.x + 1, chunkLoc.y }))
		{
			if (chunkExsists(Vector2i{ chunkLoc.x - 1, chunkLoc.y }))
			{
				if (chunkExsists(Vector2i{ chunkLoc.x, chunkLoc.y + 1}))
				{
					if (chunkExsists(Vector2i{ chunkLoc.x, chunkLoc.y - 1 }))
					{
						std::lock_guard<std::mutex> lock{ m_BuildQueueMutex };
						m_BuildQueue.insert(m_BuildQueue.begin(), copy[i]);
					}
				}
			}
		}
	}

	m_World->allowChunkDestruction();
}

void ChunkManager::updateQueues(const Camera& player)
{
	bool genEmpty{};

	{
		std::lock_guard<std::mutex> genLock{ m_GenQueueMutex };
		genEmpty = m_GenQueue.empty();
	}
	
	if (genEmpty)
		updateGenQueue(player);

	bool buildEmpty{};

	{
		std::lock_guard<std::mutex> buildLock{ m_BuildQueueMutex };
		buildEmpty = m_BuildQueue.empty();
	}
	
	if (buildEmpty)
		updateBuildQueue();
}

bool ChunkManager::isInGenQueue(Vector2i gen, int& o_Index)
{
	std::lock_guard<std::mutex> lock{ m_GenQueueMutex };

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
	std::lock_guard<std::mutex> lock{ m_BuildQueueMutex };

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

std::mutex& ChunkManager::getGenQueueMutex()
{
	return m_GenQueueMutex;
}

std::vector<Vector2i>& ChunkManager::getGenQueue()
{
	return m_GenQueue;
}

std::mutex& ChunkManager::getBuildQueueMutex()
{
	return m_BuildQueueMutex;
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
	std::lock_guard<std::mutex> lock{ m_World->getChunksMutex() };

	for (int i{}; i < m_World->getChunks().size(); ++i)
	{
		if (m_World->getChunks()[i]->getLocation() == chunkLoc)
		{
			return m_World->getChunks()[i];
		}
	}

	return nullptr;
}

void ChunkManager::getAdjacentChunks(Vector2i chunkLoc, Chunk* o_AdjacentChunks[4]) const
{
	o_AdjacentChunks[0] = getChunk(Vector2i{ chunkLoc.x + 1, chunkLoc.y });
	o_AdjacentChunks[1] = getChunk(Vector2i{ chunkLoc.x, chunkLoc.y + 1 });
	o_AdjacentChunks[2] = getChunk(Vector2i{ chunkLoc.x - 1, chunkLoc.y });
	o_AdjacentChunks[3] = getChunk(Vector2i{ chunkLoc.x, chunkLoc.y - 1 });
}

void ChunkManager::pushUploadPending(Chunk* chunk)
{
	m_World->getUploadPending().push_back(chunk);
}

void ChunkManager::clearQueues()
{
	std::lock_guard<std::mutex> genLock{ m_GenQueueMutex };
	m_GenQueue.clear();
	std::lock_guard<std::mutex> buildLock{ m_BuildQueueMutex };
	m_BuildQueue.clear();
}

void ChunkManager::setWorld(World* world)
{
	m_World = world;
}

