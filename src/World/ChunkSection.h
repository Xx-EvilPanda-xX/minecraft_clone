#ifndef CHUNK_SECTION_H
#define CHUNK_SECTION_H

#include "Block.h"
#include "../Math/Vector3i.h"

class ChunkSection
{
private:
	Block* m_Blocks;
	bool m_NeedsUpdate;
	bool m_Full;

public:
	ChunkSection();

	~ChunkSection();

	void setBlock(Vector3i loc, Block block);

	Block getBlock(Vector3i loc);

	bool isEmpty();
};

#endif
