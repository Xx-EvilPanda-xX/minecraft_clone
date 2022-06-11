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
	ChunkMesh m_SolidMesh;
	ChunkMesh m_TranslucentMesh;
	Vector2i m_Location;

	int m_CurrentSectionIndex;
	bool m_Complete;
	bool m_IsBuilt;
	bool m_Building;
	bool m_Hidden;
	std::mutex m_RemainingSectionsMutex;
	std::vector<int> m_RemainingSections;

	void resetRemaining();

	void tryAddFace(Block testBlock, Block currentBlock, Face face, Vector3i pos);

	bool threadSafeIsFinished();

public:
	Chunk(Vector2i loc, Shader& shader);

	~Chunk();

	void addSection(ChunkSection* section);

	bool buildMesh(ChunkManager& manager, int section, Chunk* adjacentChunks[4]);
	
	void finishBuilding();

	int getCurrentSectionIndex() const;

	const ChunkMesh& getSolidMesh() const;

	const ChunkMesh& getTranslucentMesh() const;

	bool isComplete() const;

	const Vector2i getLocation() const;

	ChunkSection* getSection(int index) const;

	bool isBuilt() const;

	bool isHidden() const;

	void hide();

	void show();

	std::mutex& getRemainingSectionsMutex();

	void clearMesh();
};

#endif
