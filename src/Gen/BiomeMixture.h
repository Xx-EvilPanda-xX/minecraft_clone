#ifndef BIOME_MIXTURE_H
#define BIOME_MIXTURE_H

#include <vector>
#include "Biome.h"

struct BiomeElement
{
	Biome* biome;
	double percentage;
};

class BiomeMixture
{
private:
	std::vector<BiomeElement> biomeElements;

public:
	BiomeMixture();

	~BiomeMixture();

	void addElement(Biome* biome, double percentage);

	BiomeElement getElement(int element) const;

	int getNumsElements() const;
};

#endif
