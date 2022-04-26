#ifndef BIOME_H
#define BIOME_H

#include <vector>
#include "Layer.h"
#include "../noise.h"
#include "SectionLocation.h"

class Biome
{
protected:
	std::vector<Layer> m_Layers;
	FastNoiseLite m_Noise;

	Biome();

public:
	virtual ~Biome();
	virtual const std::vector<Layer>& getLayers() const = 0;
	virtual const int** getHeightMap(SectionLocation location) = 0;
	void setNoiseParams(int octaves, float freq);
};

#endif