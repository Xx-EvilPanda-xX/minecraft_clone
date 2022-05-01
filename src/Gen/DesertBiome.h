#ifndef DESERT_BIOME_H
#define DESERT_BIOME_H

#include "Biome.h"
#include "SectionLocation.h"

class DesertBiome : public Biome
{
public:
	DesertBiome(int seed);
	~DesertBiome();

	const std::vector<Layer>& getLayers() const override;

	const std::vector<Foliage>& getFoliage() const override;

	const int** getHeightMap(Vector2i location) override;
};

#endif