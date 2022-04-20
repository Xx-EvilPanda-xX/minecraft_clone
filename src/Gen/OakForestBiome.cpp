#include "OakForestBiome.h"

OakForestBiome::OakForestBiome()
	: Biome()
{
	m_Layers.emplace_back(BlockType::Stone, -6);
	m_Layers.emplace_back(BlockType::Dirt, -1);
	m_Layers.emplace_back(BlockType::Grass, 0);
}

OakForestBiome::~OakForestBiome() = default;

const std::vector<Layer>& OakForestBiome::getLayers() const
{
	return m_Layers;
}