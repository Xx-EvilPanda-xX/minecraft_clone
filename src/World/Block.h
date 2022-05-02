#ifndef BLOCK_H
#define BLOCK_H

#include "../Player/AABB.h"
#include <string>

enum BlockType
{
	Air,
	Grass,
	Stone,
	Dirt,
	Sand,
	Gravel,
	CobbleStone,
	Wood,
	PalmLeaves,
	OakLeaves,
	Glass,
	CraftingTable,
	Planks,
	DiamondBlock,
	Water,
	Cactus,
	Snow,

	NUM_BLOCKS
};

class Block
{
private:
	BlockType m_Type;
	bool m_Transparent;
	bool m_Surface;

public:
	Block(BlockType type, bool surface);

	Block();

	BlockType getType() const;

	bool isTransparent();

	bool isSurface();

	std::string getName();
};

#endif
