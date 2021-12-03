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
	NumBlocks
};

class Block
{
private:
	BlockType m_Type;
	bool m_Transparent;

public:
	Block(BlockType type);

	Block();

	BlockType getType() const;

	bool isTransparent();

	AABB& getBounds();
};

#endif
