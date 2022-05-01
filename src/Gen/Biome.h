#ifndef BIOME_H
#define BIOME_H

#include <vector>
#include <random>
#include "Layer.h"
#include "../noise.h"
#include "SectionLocation.h"
#include "Foliage.h"

class Biome
{
protected:
	std::vector<Layer> m_Layers;
	std::vector<Foliage> m_Foliage;
	FastNoiseLite m_Noise;
	const int m_Id;

	Biome(int id, int seed);

public:
	virtual ~Biome();
	virtual const std::vector<Layer>& getLayers() const = 0;
	virtual const std::vector<Foliage>& getFoliage() const = 0;
	virtual const int** getHeightMap(Vector2i location) = 0;
	void setNoiseParams(int octaves, float freq);
	int getId();
};

#endif