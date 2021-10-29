#ifndef CHUNK_H
#define CHUNK_H

#include "ChunkSection.h"
#include "ChunkMesh.h"

extern constexpr int g_ChunkCap{ 16 };

class Chunk
{
private:
	ChunkSection* m_Sections[g_ChunkCap];
	ChunkMesh m_Mesh;
	int m_CurrentSectionIndex;
	bool m_Complete;

public:
	Chunk();

	~Chunk();

	void addSection(ChunkSection* section);

	void buildMesh();

	int getCurrentSectionIndex();

	ChunkMesh& getMesh();

	bool isComplete();
};

#endif
