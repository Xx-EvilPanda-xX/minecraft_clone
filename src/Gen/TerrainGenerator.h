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
	FastNoiseLite m_BiomeNoise1;
	FastNoiseLite m_BiomeNoise2;

	const int m_MaxTreesPerChunk{ 16 };

	void genFoliage(const Biome* biome, Vector2i pos, double currentHeight, ChunkSection* section, const SectionLocation& sectionLocation, const double** heightMap);

	void genTree(const Tree& tree, ChunkSection* section, Vector3i pos, const SectionLocation& treeSection, const double** heightMap);

	void genCactus(ChunkSection* section, Vector3i pos, const SectionLocation& cactusLocation);

	bool structureShouldBeInQueue(Vector3i pos, const SectionLocation& section, Block block);

	ChunkSection* genSection(const Biome* biomeMap[chunkSize][chunkSize], const double** heightMap, SectionLocation section);

	const BiomeMixture** getBiomeMap(Vector2i location);

	int setBiomeNoiseParams();

	template <typename T>
	T** allocMap();

	template <typename T>
	void deleteMap(const T** map);

public:
	TerrainGenerator(ChunkManager& manager);

	Chunk* generateChunk(Vector2i loc, Shader& chunkShader);

	std::vector<QueueBlock>& getBlockQueue();
};

#endif
