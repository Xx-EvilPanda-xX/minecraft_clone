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
	std::mt19937 rand;
	std::uniform_int_distribution<> die;

	void genTree(ChunkSection* section, Vector3i pos);

public:
	TerrainGenerator();

	ChunkSection* genSection(int** heightMap, int section);

	int** getHeightMap(Chunk* chunk);
};

#endif
