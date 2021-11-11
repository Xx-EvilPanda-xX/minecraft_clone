#include <iostream>
#include "ChunkSection.h"
#include "../Math/Vector3i.h"
#include "Block.h"

ChunkSection::ChunkSection() : m_Empty{ true }, m_AirBlocks{ 4096 }
{
	m_Blocks = new Block[4096]{};
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

	if (block.getType() == BlockType::Air && m_Blocks[index].getType() != BlockType::Air)
		++m_AirBlocks;
	if (block.getType() != BlockType::Air && m_Blocks[index].getType() == BlockType::Air)
		--m_AirBlocks;

	if (m_AirBlocks == 4096)
		m_Empty = true;
	else
		m_Empty = false;

	m_Blocks[index] = block;
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

bool ChunkSection::isEmpty()
{
	return m_Empty;
}