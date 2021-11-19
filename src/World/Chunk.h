#ifndef CHUNK_H
#define CHUNK_H

#include "ChunkSection.h"
#include "ChunkMesh.h"
#include "../Math/Vector2i.h"
#include "ChunkManager.h"
#include "../Render/Shader.h"

extern constexpr int g_ChunkCap{ 16 };

class Chunk
{
private:
	ChunkSection* m_Sections[g_ChunkCap];
	ChunkMesh m_Mesh;
	int m_CurrentSectionIndex;
	bool m_Complete;
	bool m_IsBuilt;
	Vector2i m_Location;
	std::vector<int> m_RemainingSections;

public:
	Chunk(Vector2i loc, Shader& shader);

	~Chunk();

	void addSection(ChunkSection* section);

	void buildMesh(ChunkManager& manager, int section);

	int getCurrentSectionIndex() const;

	const ChunkMesh& getMesh() const;

	bool isComplete() const;

	const Vector2i getLocation() const;

	ChunkSection* getSection(int index) const;

	bool isBuilt() const;

	void clearMesh();
};

#endif
