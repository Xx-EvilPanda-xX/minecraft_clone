#ifndef GRAVEL_PLAINS_BIOME_H
#define GRAVEL_PLAINS_BIOME_H

#include "Biome.h"
#include "SectionLocation.h"
#include "../Math/Vector2i.h"

class GravelPlainsBiome : public Biome
{
public:
	GravelPlainsBiome(int seed);
	~GravelPlainsBiome();

	const std::vector<Layer>& getLayers() const override;

	const std::vector<Foliage>& getFoliage() const override;

	double** getHeightMap(Vector2i location) override;
};

#endif