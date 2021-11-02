#ifndef TERRAIN_GENERATOR_H
#define TERRAIN_GENERATOR_H

#include "../World/ChunkSection.h"
#include <glm/vec2.hpp>

class TerrainGenerator
{
public:
	TerrainGenerator();

	~TerrainGenerator();

	ChunkSection* genSection(glm::vec2  loc, int section);
};

#endif
