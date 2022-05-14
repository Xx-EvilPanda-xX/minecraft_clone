#ifndef BIOME_MAP_H
#define BIOME_MAP_H

#include "Biome.h"

struct BiomeDefinition
{
	int top;
	int bottom;
};

class Biomes
{
private:
	Biomes();
	~Biomes();

public:
	static const BiomeDefinition s_Mountains;
	static const BiomeDefinition s_GravelPlains;
	static const BiomeDefinition s_OakForest;
	static const BiomeDefinition s_Plains;
	static const BiomeDefinition s_Desert;
};


#endif