#ifndef TERRAIN_GENERATOR_H
#define TERRAIN_GENERATOR_H

#include <random>
#include "../World/ChunkSection.h"
#include "../World/Chunk.h"
#include "../World/ChunkManager.h"
#include "../noise.h"
#include <unordered_map>

struct SectionLocation
{
	int sectionIndex;
	Vector2i worldPos;
};

struct QueueBlock
{
	Block block;
	Vector3i sectionRelativePos;
	SectionLocation loc;
};

class TerrainGenerator
{
private:
	FastNoiseLite m_Noise;
	std::mt19937 m_Rand;
	std::uniform_int_distribution<> m_Die;
	std::vector<QueueBlock> m_BlockQueue;
	ChunkManager& m_Manager;

	const int m_MaxTreesPerChunk{ 16 };

	void genTree(ChunkSection* section, Vector3i pos, const SectionLocation& treeSection);

	bool structureShouldBeInQueue(Vector3i pos, const SectionLocation& section, Block block);

public:
	static Vector3i treeLeaves[13];

	TerrainGenerator(ChunkManager& manager);

	ChunkSection* genSection(int** heightMap, SectionLocation section);

	int** getHeightMap(Chunk* chunk);

	std::vector<QueueBlock>& getBlockQueue();
};

#endif
