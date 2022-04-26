#include "Biome.h"

Biome::Biome() = default;

Biome::~Biome() = default;

void Biome::setNoiseParams(int octaves, float freq)
{
	m_Noise.SetNoiseType(FastNoiseLite::NoiseType::NoiseType_OpenSimplex2S);
	m_Noise.SetCellularDistanceFunction(FastNoiseLite::CellularDistanceFunction::CellularDistanceFunction_Hybrid);
	m_Noise.SetCellularReturnType(FastNoiseLite::CellularReturnType::CellularReturnType_Distance2);
	m_Noise.SetFractalType(FastNoiseLite::FractalType::FractalType_FBm);
	m_Noise.SetFractalOctaves(octaves);
	m_Noise.SetFrequency(freq);
	m_Noise.SetSeed(1377);
}
