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
#include "BiomeMixture.h"
#include "../noise.h"
#include "Random.h"

constexpr int chunkSize{ 16 };

struct QueueBlock
{
	Block block;
	Vector3i sectionRelativePos;
	SectionLocation loc;
};

class TerrainGenerator
{
private:
	Random m_Rand;
	int m_Seed;
	std::vector<QueueBlock> m_BlockQueue;
	ChunkManager& m_Manager;
	FastNoiseLite m_BiomeNoise;

	const int m_MaxTreesPerChunk{ 16 };

	void genTree(const Tree& tree, ChunkSection* section, Vector3i pos, const SectionLocation& treeSection, const int** heightMap);

	bool structureShouldBeInQueue(Vector3i pos, const SectionLocation& section, Block block);

	ChunkSection* genSection(const Biome* biomeMap[chunkSize][chunkSize], const int** heightMap, SectionLocation section);

	const BiomeMixture** getBiomeMap(Vector2i location);

	void addHeightMaps(int** dest, const int** map1, const int** map2);

	int** allocHeightMap();

	void deleteHeightMap(const int** map);

public:
	TerrainGenerator(ChunkManager& manager);

	Chunk* generateChunk(Vector2i loc, Shader& chunkShader);

	std::vector<QueueBlock>& getBlockQueue();
};

#endif
