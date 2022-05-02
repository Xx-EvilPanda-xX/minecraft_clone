#ifndef OAK_FOREST_BIOME_H
#define OAK_FOREST_BIOME_H

#include "Biome.h"
#include "SectionLocation.h"
#include "../Math/Vector2i.h"

class OakForestBiome : public Biome
{
public:
	OakForestBiome(int seed);
	~OakForestBiome();

	const std::vector<Layer>& getLayers() const override;

	const std::vector<Foliage>& getFoliage() const override;

	const double** getHeightMap(Vector2i location) override;
};

#endif