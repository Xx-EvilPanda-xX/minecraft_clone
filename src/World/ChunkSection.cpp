#include <iostream>
#include "ChunkSection.h"
#include "../Math/Vector3i.h"
#include "Block.h"

ChunkSection::ChunkSection() 
	: m_Empty{ true },
	m_Full{ false },
	m_OpaqueBlocks{ 0 },
	m_AirBlocks{ 4096 }
{
	m_Blocks = new Block[4096]{};
}

ChunkSection::~ChunkSection()
{
	delete[] m_Blocks;
	m_Blocks = nullptr;
}

void ChunkSection::setBlock(Vector3i loc, BlockType type, bool surface)
{
	int index{};

	index += loc.z;
	index += loc.y * 16;
	index += loc.x * (16 * 16);

	if (index >= 4096 || index < 0)
	{
		return;
	}

	Block block{ type, surface };

	if (block.getType() == BlockType::Air && m_Blocks[index].getType() != BlockType::Air)
		++m_AirBlocks;
	if (block.getType() != BlockType::Air && m_Blocks[index].getType() == BlockType::Air)
		--m_AirBlocks;

	if (!block.isTransparent() && m_Blocks[index].isTransparent())
		++m_OpaqueBlocks;
	if (block.isTransparent() && !m_Blocks[index].isTransparent())
		--m_OpaqueBlocks;

	if (m_AirBlocks == 4096)
		m_Empty = true;
	else
		m_Empty = false;

	if (m_OpaqueBlocks == 4096)
		m_Full = true;
	else
		m_Full = false;

	m_Blocks[index] = block;
}

Block ChunkSection::getBlock(Vector3i loc) const
{
	int index{};

	index += loc.z;
	index += loc.y * 16;
	index += loc.x * (16 * 16);

	if (index >= 4096 || index < 0)
	{
		return Block{};
	}

	return m_Blocks[index];
}

bool ChunkSection::isEmpty() const
{
	return m_Empty;
}

bool ChunkSection::isFull() const
{
	return m_Full;
}