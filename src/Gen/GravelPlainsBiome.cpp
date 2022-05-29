#include "GravelPlainsBiome.h"
#include "../Constants.h"
#include "Random.h"

constexpr int ID{ 4 };

GravelPlainsBiome::GravelPlainsBiome(int seed)
	: Biome(ID, seed)
{
	m_Layers.emplace_back(Block{ BlockType::Stone, false }, -6, 0, true);
	m_Layers.emplace_back(Block{ BlockType::Gravel, false }, 0, 0, true);
	addWaterLayer();

	//TODO: add new foliage type for gravel plains
	m_Foliage.emplace_back(Foliage::FoliageType::BUSH, 0.025, constants::waterLevel + 3, 255);
	m_Foliage.emplace_back(Foliage::FoliageType::ROSE, 0.01, constants::waterLevel + 5, 255);

	setNoiseParams(7, 0.00355f);
}

GravelPlainsBiome::~GravelPlainsBiome() = default;

const std::vector<Layer>& GravelPlainsBiome::getLayers() const
{
	return m_Layers;
}

const double** GravelPlainsBiome::getHeightMap(Vector2i location)
{
	double** heightMap = new double* [16];

	for (int i{}; i < 16; ++i)
	{
		heightMap[i] = new double[16];
	}

	int chunkX{ location.x * 16 };
	int chunkY{ location.y * 16 };

	for (int i{}; i < 16; ++i)
	{
		for (int j{}; j < 16; ++j)
		{
			if (constants::flatWorld)
				heightMap[i][j] = constants::flatWorldHeight;
			else
				heightMap[i][j] = (static_cast<double>(m_Noise.GetNoise<float>(static_cast<float>(chunkX + i), static_cast<float>(chunkY + j))) / 2.0 + 0.5) * 100.0;

			if (heightMap[i][j] < constants::waterLevel)
			{
				heightMap[i][j] += constants::beachSize;
				if (heightMap[i][j] > constants::waterLevel)
					heightMap[i][j] = constants::waterLevel;
			}
		}
	}

	return (const double**)heightMap;
}

const std::vector<Foliage>& GravelPlainsBiome::getFoliage() const
{
	return m_Foliage;
}