#ifndef PLAINS_BIOME_H
#define PLAINS_BIOME_H

#include "Biome.h"
#include "SectionLocation.h"
#include "../Math/Vector2i.h"

class PlainsBiome : public Biome
{
public:
	PlainsBiome(int seed);
	~PlainsBiome();

	const std::vector<Layer>& getLayers() const override;

	const std::vector<Foliage>& getFoliage() const override;

	const int** getHeightMap(Vector2i location) override;
};

#endif