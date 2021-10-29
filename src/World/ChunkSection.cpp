#include <iostream>
#include "ChunkSection.h"
#include "../Math/Vector3i.h"
#include "Block.h"

ChunkSection::ChunkSection()
{
}

ChunkSection::~ChunkSection()
{
}

void ChunkSection::setBlock(Vector3i loc, Block block)
{
	if (loc.m_X > 15 || loc.m_Y > 15 || loc.m_Z > 15)
	{
		std::cout << "Block index out of range!\n";
		return;
	}
	int index{};

	index += loc.m_X;
	index += loc.m_Z * 16;
	index += loc.m_Y * (16 * 16);

	m_Blocks[index] = block;

	m_NeedsUpdate = true;
}

Block ChunkSection::getBlock(Vector3i loc)
{
	if (loc.m_X > 15 || loc.m_Y > 15 || loc.m_Z > 15)
	{
		std::cout << "Block index out of range!\n";
		return;
	}
	int index{};

	index += loc.m_X;
	index += loc.m_Z * 16;
	index += loc.m_Y * (16 * 16);

	return m_Blocks[index];
}

bool ChunkSection::isFull()
{
	if (m_NeedsUpdate)
	{
		for (int i{}; i < 4096; ++i)
		{
			if (m_Blocks->getType() == BlockType::Air)
			{
				m_NeedsUpdate = false;
				m_Full = false;
				return m_Full;
			}
		}
		m_Full = true;
	}

	return m_Full;
}

