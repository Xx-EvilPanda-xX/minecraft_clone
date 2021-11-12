#ifndef TERRAIN_GENERATOR_H
#define TERRAIN_GENERATOR_H

#include <random>
#include "../World/ChunkSection.h"

class TerrainGenerator
{
private:
	std::mt19937 rand;
	std::uniform_int_distribution<> die;

public:
	TerrainGenerator();

	ChunkSection* genSection(int** heightMap, int section);

	int** getHeightMap();
};

#endif
