#ifndef BIOME_H
#define BIOME_H

#include <vector>
#include "Layer.h"

class Biome
{
protected:
	std::vector<Layer> m_Layers;

	Biome();

public:
	virtual ~Biome();
	virtual const std::vector<Layer>& getLayers() const = 0;
	//virtual const int** getHeightMap() = 0;
};

#endif