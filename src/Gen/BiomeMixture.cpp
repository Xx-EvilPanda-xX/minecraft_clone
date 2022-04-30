#include "BiomeMixture.h"

BiomeMixture::BiomeMixture() = default;

BiomeMixture::~BiomeMixture()
{
	for (int i{}; i < biomeElements.size(); ++i)
	{
		delete biomeElements.at(i).biome;
	}
}

void BiomeMixture::addElement(Biome* biome, double percentage)
{
	biomeElements.push_back(BiomeElement{ biome, percentage });
}

BiomeElement BiomeMixture::getElement(int element) const
{
	if (element >= biomeElements.size())
		return BiomeElement{ nullptr, 0.0 };

	return biomeElements.at(element);
}

int BiomeMixture::getNumsElements() const
{
	return biomeElements.size();
}