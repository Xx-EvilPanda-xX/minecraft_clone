#include <iostream>
#include "ChunkSection.h"
#include "../Math/Vector3i.h"
#include "Block.h"

ChunkSection::ChunkSection() 
	: m_Empty{ true },
	m_Full{ false },
	m_OpaqueBlocks{ 0 },
	m_AirBlocks{ g_ChunkSectionCapacity }
{
}

ChunkSection::~ChunkSection()
{
}

void ChunkSection::setBlock(Vector3i loc, BlockType type, bool surface)
{
	int index{};

	if (loc.x > 15 || loc.y > 15 || loc.z > 15 ||
		loc.x < 0 || loc.y < 0 || loc.z < 0)
	{
		return;
	}

	index += loc.z;
	index += loc.y * 16;
	index += loc.x * (16 * 16);

	Block block{ type, surface };

	if (block.getType() == BlockType::Air && m_Blocks[index].getType() != BlockType::Air)
		++m_AirBlocks;
	if (block.getType() != BlockType::Air && m_Blocks[index].getType() == BlockType::Air)
		--m_AirBlocks;

	if (!block.isTransparent() && m_Blocks[index].isTransparent())
		++m_OpaqueBlocks;
	if (block.isTransparent() && !m_Blocks[index].isTransparent())
		--m_OpaqueBlocks;

	if (m_AirBlocks == g_ChunkSectionCapacity)
		m_Empty = true;
	else
		m_Empty = false;

	if (m_OpaqueBlocks == g_ChunkSectionCapacity)
		m_Full = true;
	else
		m_Full = false;

	m_Blocks[index] = block;
}

Block ChunkSection::getBlock(Vector3i loc) const
{
	int index{};

	if (loc.x > 15 || loc.y > 15 || loc.z > 15 ||
		loc.x < 0 || loc.y < 0 || loc.z < 0)
	{
		return Block{};
	}

	index += loc.z;
	index += loc.y * 16;
	index += loc.x * (16 * 16);

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