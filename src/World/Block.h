#ifndef BLOCK_H
#define BLOCK_H

enum BlockType
{
	Air,
	Grass,
	Stone,
	Dirt,
	CobbleStone,
	NumBlocks
};

class Block
{
private:
	BlockType m_Type;

public:
	Block(BlockType type);

	Block();

	BlockType getType() const;
};

#endif
