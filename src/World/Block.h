#ifndef BLOCK_H
#define BLOCK_H

#include "../Player/AABB.h"

enum BlockType
{
	Air,
	Grass,
	Stone,
	Dirt,
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
	AABB m_Bounds;
	//Vector3i m_Velocity;
	bool m_Transparent;

public:
	Block(BlockType type, AABB bounds);

	Block();

	BlockType getType() const;

	bool isTransparent();

	AABB& getBounds();
};

#endif
