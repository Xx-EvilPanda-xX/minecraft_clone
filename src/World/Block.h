#ifndef BLOCK_H
#define BLOCK_H

#include "../Player/AABB.h"

enum BlockType
{
	Air,
	Grass,
	Stone,
	Dirt,
	Sand,
	CobbleStone,
	Wood,
	Leaves,
	Glass,
	CraftingTable,
	Planks,
	DiamondBlock,
	Water,

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
};

#endif
