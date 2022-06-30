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
	Flowers,
	Glass,
	CraftingTable,
	Planks,
	DiamondBlock,
	Water,
	Cactus,
	Snow,
	TallGrass,
	Shrub,
	Rose,

	NUM_BLOCKS
};

class Block
{
private:
	//enums use 32 bits, char is only 8. I'll likely never add more than 256 blocks.
	char m_Type;
	bool m_Transparent;
	bool m_Surface;
	bool m_FoliageMesh;

public:
	Block(BlockType type, bool surface);

	Block();

	BlockType getType() const;

	bool isTransparent();

	bool isSurface();

	bool isFoliageMesh();

	std::string getName();

	friend class ChunkSaver;
	friend class ChunkLoader;
};

#endif
