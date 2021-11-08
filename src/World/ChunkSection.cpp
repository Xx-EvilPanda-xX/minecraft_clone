#include <iostream>
#include "ChunkSection.h"
#include "../Math/Vector3i.h"
#include "Block.h"

ChunkSection::ChunkSection() : m_Empty{ false }, m_NeedsUpdate{ true }
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

	index += loc.z;
	index += loc.y * 16;
	index += loc.x * (16 * 16);

	if (index >= 4096 || index < 0)
	{
		return;
	}

	m_Blocks[index] = block;

	m_NeedsUpdate = true;
}

Block ChunkSection::getBlock(Vector3i loc)
{
	int index{};

	index += loc.z;
	index += loc.y * 16;
	index += loc.x * (16 * 16);

	if (index >= 4096 || index < 0)
	{
		return Block{ BlockType::Air };
	}

	return m_Blocks[index];
}