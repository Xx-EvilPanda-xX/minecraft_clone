#include "Block.h"

Block::Block(BlockType type, bool surface) 
	: m_Type{ static_cast<char>(type) },
	m_Surface{ surface }
{
	m_Transparent = false;
	m_FoliageMesh = false;

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
	case Flowers:
		m_Transparent = true;
		break;
	case Glass:
		m_Transparent = true;
		break;
	case TallGrass:
		m_Transparent = true;
		m_FoliageMesh = true;
		break;
	case Shrub:
		m_Transparent = true;
		m_FoliageMesh = true;
		break;
	case Rose:
		m_Transparent = true;
		m_FoliageMesh = true;
	default:
		break;
	}
}

Block::Block()
{
	m_Type = static_cast<char>(BlockType::Air);
	m_Transparent = true;
	m_Surface = false;
}

BlockType Block::getType() const
{
	return static_cast<BlockType>(m_Type);
}

bool Block::isTransparent()
{
	return m_Transparent;
}

bool Block::isSurface()
{
	return m_Surface;
}

bool Block::isFoliageMesh()
{
	return m_FoliageMesh;
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
	case Flowers:
		return "Flowers";
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
	case TallGrass:
		return "TallGrass";
	case Shrub:
		return "Shrub";
	case Rose:
		return "Rose";
	case RedWool:
		return "RedWool";
	case OrangeWool:
		return "OrangeWool";
	case YellowWool:
		return "YellowWool";
	case GreenWool:
		return "GreenWool";
	case SkyBlueWool:
		return "SkyBlueWool";
	case BlueWool:
		return "BlueWool";
	case PurpleWool:
		return "PurpleWool";
	case PinkWool:
		return "PinkWool";
	case BrownWool:
		return "BrownWool";
	case BlackWool:
		return "BlackWool";
	default:
		return "Unknown";
	}
}