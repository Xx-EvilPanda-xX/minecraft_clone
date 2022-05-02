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
	case PalmLeaves:
		m_Transparent = true;
		break;
	case OakLeaves:
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
	m_Surface = false;
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

std::string Block::getName()
{
	switch (m_Type)
	{
	case Air:
		return "Air";
	case Grass:
		return "Grass";
	case Stone:
		return "Stone";
	case Dirt:
		return "Dirt";
	case Sand:
		return "Sand";
	case Gravel:
		return "Gravel";
	case CobbleStone:
		return "CobbleStone";
	case Wood:
		return "Wood";
	case PalmLeaves:
		return "PalmLeaves";
	case OakLeaves:
		return "OakLeaves";
	case Glass:
		return "Glass";
	case CraftingTable:
		return "CraftingTable";
	case Planks:
		return "Planks";
	case DiamondBlock:
		return "DiamondBlock";
	case Water:
		return "Water";
	case Cactus:
		return "Cactus";
	case Snow:
		return "Snow";
	default:
		return "Unknown";
	}
}