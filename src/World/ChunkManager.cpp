#include <cmath>
#include "ChunkManager.h"
#include "Block.h"
#include "../Math/Vector3i.h"
#include "../Math/Vector2i.h"
#include "World.h"

ChunkManager::ChunkManager(World* world) : m_World{ world }
{
}

ChunkManager::ChunkManager() = default;

ChunkManager::~ChunkManager()
{
}

void ChunkManager::setWorldBlock(Vector3i loc, Block block)
{
	int chunkIndex{};
	int sectionIndex{};

	Vector3i sectionLocal{ loc.x, loc.y, loc.z };

	while (sectionLocal.x > 15 || sectionLocal.x < -15)
	{
		if (sectionLocal.x > 0) sectionLocal.x -= 16;
		else sectionLocal.x += 16;
	}

	while (sectionLocal.y > 15 || sectionLocal.y < -15)
	{
		if (sectionLocal.y > 0) sectionLocal.y -= 16;
		else sectionLocal.y += 16;
	}

	while (sectionLocal.z > 15 || sectionLocal.z < -15)
	{
		if (sectionLocal.z > 0) sectionLocal.z -= 16;
		else sectionLocal.z += 16;
	}

	sectionLocal.x = std::abs(sectionLocal.x);
	sectionLocal.y = std::abs(sectionLocal.y);
	sectionLocal.z = std::abs(sectionLocal.z);

	Vector2i chunkLocation{ loc.x - sectionLocal.x, loc.z - sectionLocal.z };
	for (int i{}; i < m_World->getChunks().size(); ++i)
	{
		if (m_World->getChunks()[i]->getLocation().x == chunkLocation.x && m_World->getChunks()[i]->getLocation().y == chunkLocation.y)
		{
			chunkIndex = i;
			break;
		}
	}
	sectionIndex = loc.y / 16;

	if (chunkIndex > m_World->getChunks().size())
	{
		std::cout << "Chunk at " << chunkLocation.x << ", " << chunkLocation.y << "does not exist!";
	}

	m_World->getChunks()[chunkIndex]->getSection(sectionIndex)->setBlock(Vector3i{ sectionLocal.x, sectionLocal.y, sectionLocal.z }, block);
}

Block ChunkManager::getWorldBlock(Vector3i loc)
{
	int chunkIndex{};
	int sectionIndex{};

	Vector3i sectionLocal{ loc.x, loc.y, loc.z };

	while (sectionLocal.x > 15 || sectionLocal.x < -15)
	{
		if (sectionLocal.x > 0) sectionLocal.x -= 16;
		else sectionLocal.x += 16;
	}

	while (sectionLocal.y > 15 || sectionLocal.y < -15)
	{
		if (sectionLocal.y > 0) sectionLocal.y -= 16;
		else sectionLocal.y += 16;
	}

	while (sectionLocal.z > 15 || sectionLocal.z < -15)
	{
		if (sectionLocal.z > 0) sectionLocal.z -= 16;
		else sectionLocal.z += 16;
	}

	sectionLocal.x = std::abs(sectionLocal.x);
	sectionLocal.y = std::abs(sectionLocal.y);
	sectionLocal.z = std::abs(sectionLocal.z);

	Vector2i chunkLocation{ loc.x - sectionLocal.x, loc.z - sectionLocal.z };
	for (int i{}; i < m_World->getChunks().size(); ++i)
	{
		if (m_World->getChunks()[i]->getLocation().x == chunkLocation.x && m_World->getChunks()[i]->getLocation().y == chunkLocation.y)
		{
			chunkIndex = i;
			break;
		}
	}
	sectionIndex = loc.y / 16;

	if (chunkIndex > m_World->getChunks().size())
	{
		std::cout << "Chunk at " << chunkLocation.x << ", " << chunkLocation.y << "does not exist!";
	}

	return m_World->getChunks()[chunkIndex]->getSection(sectionIndex)->getBlock(Vector3i{ sectionLocal.x, sectionLocal.y, sectionLocal.z } );
}