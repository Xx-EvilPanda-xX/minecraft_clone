#ifndef BLOCK_H
#define BLOCK_H

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
};

#endif
