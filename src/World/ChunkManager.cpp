#include <cmath>
#include <chrono>
#include "ChunkManager.h"
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

	Vector3i sectionLocal{ loc.x % 16, loc.y % 16, loc.z % 16 };
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
		return Block{};

	sectionIndex = loc.y / 16;

	if (sectionIndex > 15)
		return Block{};

	return m_World->getChunks()[chunkIndex]->getSection(sectionIndex)->getBlock(sectionLocal);
}

bool ChunkManager::chunkExsists(Vector3i loc)
{
	return false;
}