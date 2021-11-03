#include <iostream>
#include "ChunkSection.h"
#include "../Math/Vector3i.h"
#include "Block.h"

ChunkSection::ChunkSection() : m_Full{ false }, m_NeedsUpdate{ true }
{
	m_Blocks = new Block[4096];
}

ChunkSection::~ChunkSection()
{
	delete[] m_Blocks;
}

void ChunkSection::setBlock(Vector3i loc, Block block)
{
	int index{};

	index += loc.x;
	index += loc.z * 16;
	index += loc.y * (16 * 16);

	if (index >= 4096 || index < 0)
	{
		std::cout << "Block index out of range! " << index << "\n";
		return;
	}

	m_Blocks[index] = block;

	m_NeedsUpdate = true;
}

Block ChunkSection::getBlock(Vector3i loc)
{
	int index{};

	index += loc.x;
	index += loc.z * 16;
	index += loc.y * (16 * 16);

	if (index >= 4096 || index < 0)
	{
		std::cout << "Block index out of range! " << index << "\n";
		return Block{ BlockType::Air };
	}

	return m_Blocks[index];
}

bool ChunkSection::isEmpty()
{
	if (m_NeedsUpdate)
	{
		for (int i{}; i < 4096; ++i)
		{
			if (m_Blocks[i].getType() != BlockType::Air)
			{
				m_NeedsUpdate = false;
				m_Full = false;
				return m_Full;
			}
		}
		m_NeedsUpdate = false;
		m_Full = true;
	}

	return m_Full;
}

