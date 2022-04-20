#ifndef PLAINS_BIOME_H
#define PLAINS_BIOME_H

#include "Biome.h"

class OakForestBiome : public Biome
{
public:
	OakForestBiome();
	~OakForestBiome();

	const std::vector<Layer>& getLayers() const override;
};

#endif