#ifndef TERRAIN_GENERATOR_H
#define TERRAIN_GENERATOR_H

#include "../World/ChunkSection.h"
#include "../Math/Vector2i.h"

class TerrainGenerator
{
public:
	TerrainGenerator();

	~TerrainGenerator();

	ChunkSection* genSection(Vector2i loc, int section);
};

#endif
