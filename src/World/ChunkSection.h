#ifndef CHUNK_SECTION_H
#define CHUNK_SECTION_H

#include "Block.h"
#include <glm/vec3.hpp>

class ChunkSection
{
private:
	Block* m_Blocks;
	bool m_NeedsUpdate;
	bool m_Full;

public:
	ChunkSection();

	~ChunkSection();

	void setBlock(glm::vec3 loc, Block block);

	Block getBlock(glm::vec3 loc);

	bool isEmpty();
};

#endif
