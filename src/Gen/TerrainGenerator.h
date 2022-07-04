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
#include "QueueBlock.h"

constexpr int chunkSize{ 16 };

class TerrainGenerator
{
private:
	Random m_Rand;
	int m_Seed;

	//no mutex since block queue is push to and popped from in chunking thread
	std::vector<QueueBlock> m_BlockQueue;
	ChunkManager& m_Manager;
	FastNoiseLite m_BiomeNoise1;
	FastNoiseLite m_BiomeNoise2;

	const int m_MaxTreesPerChunk{ 16 };

	void genFoliage(const Biome* biome, Vector2i pos,
				ChunkSection* section, const SectionLocation& sectionLocation,
				double heightMap[chunkSize][chunkSize]);

	void genTree(const Tree& tree, ChunkSection* section,
				Vector3i pos, const SectionLocation& sectionLocation,
				double heightMap[chunkSize][chunkSize]);

	void genCactus(ChunkSection* section, Vector3i pos, const SectionLocation& sectionLocation);

	bool structureShouldBeInQueue(Vector3i pos, const SectionLocation& sectionLocation, Block block);

	ChunkSection* genSection(Biome* biomeMap[chunkSize][chunkSize],
				double heightMap[chunkSize][chunkSize],
				SectionLocation sectionLocation);

	BiomeMixture** getBiomeMap(Vector2i location);

	int setBiomeNoiseParams(const int* s);

	template <typename T>
	T** allocMap();

	template <typename T>
	void deleteMap(T** map);

public:
	TerrainGenerator(ChunkManager& manager);

	Chunk* generateChunk(Vector2i loc, Shader& chunkShader,
					std::pair<std::mutex&,
					std::vector<unsigned int>&> bufferDestroyQueue);

	std::vector<QueueBlock>& getBlockQueue();

	int getSeed() const;

	void setSeed(int newSeed);
};

#endif
