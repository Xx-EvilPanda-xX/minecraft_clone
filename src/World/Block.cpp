#include "Block.h"

Block::Block(BlockType type, bool surface) 
	: m_Type{ type },
	m_Surface{ surface }
{
	m_Transparent = false;

	switch (type)
	{
	case Air:
		m_Transparent = true;
		break;
	case Water:
		m_Transparent = true;
		break;
	case Leaves:
		m_Transparent = true;
		break;
	case Glass:
		m_Transparent = true;
		break;
	default:
		break;
	}
}

Block::Block()
{
	m_Type = BlockType::Air;
	m_Transparent = true;
}

BlockType Block::getType() const
{
	return m_Type;
}

bool Block::isTransparent()
{
	return m_Transparent;
}

bool Block::isSurface()
{
	return m_Surface;
}