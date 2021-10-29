#ifndef BLOCK_H
#define BLOCK_H

enum class BlockType
{
	Air,
	Grass,
	Stone,
};

class Block
{
private:
	BlockType m_Type;

public:
	Block(BlockType type);

	Block();

	~Block();

	BlockType getType();
};

#endif
