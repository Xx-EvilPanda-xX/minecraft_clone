#ifndef MOUNTAIN_BIOME_H
#define MOUNTAIN_BIOME_H

#include "Biome.h"
#include "SectionLocation.h"
#include "../Math/Vector2i.h"

class MountainBiome : public Biome
{
public:
	MountainBiome(int seed);
	~MountainBiome();

	const std::vector<Layer>& getLayers() const override;

	const std::vector<Foliage>& getFoliage() const override;

	const double** getHeightMap(Vector2i location) override;
};

#endif