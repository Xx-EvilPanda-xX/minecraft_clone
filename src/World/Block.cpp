#include "Block.h"

Block::Block(BlockType type) : m_Type{ type }
{
}

Block::Block()
{
	m_Type = BlockType::Air;
}

Block::~Block() = default;

BlockType Block::getType()
{
	return m_Type;
}