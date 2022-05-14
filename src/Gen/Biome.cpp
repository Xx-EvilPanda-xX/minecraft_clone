#include "Biome.h"

Biome::Biome(int id, int seed)
	: m_Id{ id }
{
	m_Noise.SetNoiseType(FastNoiseLite::NoiseType::NoiseType_OpenSimplex2S);
	m_Noise.SetFractalType(FastNoiseLite::FractalType::FractalType_FBm);
	m_Noise.SetSeed(seed);
}

Biome::~Biome() = default;

void Biome::setNoiseParams(int octaves, float freq)
{
	m_Noise.SetFractalOctaves(octaves);
	m_Noise.SetFrequency(freq);
}

int Biome::getId()
{
	return m_Id;
}

void Biome::addWaterLayer()
{
	m_Layers.emplace_back(Block{ BlockType::Water, false }, constants::waterLevel - 1, 0, false);
	m_Layers.emplace_back(Block{ BlockType::Water, true }, constants::waterLevel, 0, false);
}