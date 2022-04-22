#ifndef TERRAIN_GENERATOR_H
#define TERRAIN_GENERATOR_H

#include <random>
#include "../World/ChunkSection.h"
#include "../World/Chunk.h"
#include "../World/ChunkManager.h"
#include <unordered_map>
#include "Tree.h"
#include "../World/Block.h"
#include "Biome.h"
#include "SectionLocation.h"

struct QueueBlock
{
	Block block;
	Vector3i sectionRelativePos;
	SectionLocation loc;
};

class TerrainGenerator
{
private:
	std::mt19937 m_Rand;
	std::uniform_int_distribution<> m_Die;
	std::vector<QueueBlock> m_BlockQueue;
	ChunkManager& m_Manager;

	const int m_MaxTreesPerChunk{ 16 };

	void genTree(const Tree& tree, ChunkSection* section, Vector3i pos, const SectionLocation& treeSection, const int** heightMap);

	bool structureShouldBeInQueue(Vector3i pos, const SectionLocation& section, Block block);

	ChunkSection* genSection(const std::vector<Layer>& biomeLayers, const int** heightMap, SectionLocation section);

public:
	TerrainGenerator(ChunkManager& manager);

	Chunk* generateChunk(Vector2i loc, Shader& chunkShader);

	std::vector<QueueBlock>& getBlockQueue();
};

#endif
