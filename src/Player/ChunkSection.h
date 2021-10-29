#ifndef CHUNK_SECTION_H
#define CHUNK_SECTION_H

#include "Block.h"

class ChunkSection
{
public:
	Block m_Blocks[4096];

	ChunkSection();

	~ChunkSection();
};

#endif
