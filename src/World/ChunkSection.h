#ifndef CHUNK_SECTION_H
#define CHUNK_SECTION_H

#include "Block.h"
#include "../Math/Vector3i.h"
#include "../Player/AABB.h"

constexpr int g_ChunkSectionCapacity{ 4096 };

class ChunkSection
{
private:
	Block m_Blocks[g_ChunkSectionCapacity];
	int m_OpaqueBlocks;
	int m_AirBlocks;
	bool m_Empty;
	bool m_Full;

public:
	ChunkSection();

	~ChunkSection();

	void setBlock(Vector3i loc, BlockType type, bool surface);

	Block getBlock(Vector3i loc) const;

	bool isEmpty() const;

	bool isFull() const;

	friend class ChunkSaver;
	friend class ChunkLoader;
};

#endif
