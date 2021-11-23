#include "Block.h"

Block::Block(BlockType type, AABB bounds) 
	: m_Type{ type },
	m_Bounds{ bounds }
{
	m_Transparent = false;

	switch (type)
	{
	case Air:
		m_Transparent = true;
		break;
	case Grass:
		break;
	case Stone:
		break;
	case Dirt:
		break;
	case CobbleStone:
		break;
	case Wood:
		break;
	case Leaves:
		m_Transparent = true;
		break;
	case Glass:
		m_Transparent = true;
		break;
	case NumBlocks:
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

AABB& Block::getBounds()
{
	return m_Bounds;
}