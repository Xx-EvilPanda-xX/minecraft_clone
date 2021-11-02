#ifndef CHUNK_H
#define CHUNK_H

#include "ChunkSection.h"
#include "ChunkMesh.h"
#include <glm/vec2.hpp>
#include "ChunkManager.h"

extern constexpr int g_ChunkCap{ 16 };

class Chunk
{
private:
	ChunkSection* m_Sections[g_ChunkCap];
	ChunkMesh m_Mesh;
	int m_CurrentSectionIndex;
	bool m_Complete;
	glm::vec2 m_Location;

public:
	Chunk(glm::vec2 loc);

	~Chunk();

	void addSection(ChunkSection* section);

	void buildMesh(ChunkManager& manager);

	int getCurrentSectionIndex();

	ChunkMesh& getMesh();

	bool isComplete();

	glm::vec2  getLocation();

	ChunkSection* getSection(int index);
};

#endif
