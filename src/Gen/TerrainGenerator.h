#ifndef TERRAIN_GENERATOR_H
#define TERRAIN_GENERATOR_H

#include "../World/ChunkSection.h"

class TerrainGenerator
{
public:
	TerrainGenerator();

	~TerrainGenerator();

	ChunkSection* genSection(int section);
};

#endif
