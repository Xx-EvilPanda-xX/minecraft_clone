#include "Block.h"

Block::Block(BlockType type) 
	: m_Type{ type }
{
}

Block::Block()
{
	m_Type = BlockType::Air;
}

BlockType Block::getType() const
{
	return m_Type;
}