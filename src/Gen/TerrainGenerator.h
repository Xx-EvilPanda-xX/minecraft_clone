#ifndef TERRAIN_GENERATOR_H
#define TERRAIN_GENERATOR_H

#include <random>
#include "../World/ChunkSection.h"
#include "../World/Chunk.h"
#include "../noise.h"

class TerrainGenerator
{
private:
	FastNoiseLite m_Noise;
	std::mt19937 m_Rand;
	std::uniform_int_distribution<> m_Die;

	const int m_MaxTreesPerChunk{ 2 };

	void genTree(ChunkSection* section, Vector3i pos);

public:
	TerrainGenerator();

	ChunkSection* genSection(int** heightMap, int section);

	int** getHeightMap(Chunk* chunk);
};

#endif
