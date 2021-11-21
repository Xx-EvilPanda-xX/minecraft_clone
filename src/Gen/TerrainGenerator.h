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

public:
	TerrainGenerator();

	ChunkSection* genSection(int** heightMap, int section);

	int** getHeightMap(Chunk* chunk);
};

#endif
