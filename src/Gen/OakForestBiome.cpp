#include "OakForestBiome.h"
#include "../Constants.h"
#include "Random.h"

constexpr int ID{ 0 };

OakForestBiome::OakForestBiome(int seed)
	: Biome(ID, seed)
{
	m_Layers.emplace_back(Block{ BlockType::Stone, false }, -6, 0, true);
	m_Layers.emplace_back(Block{ BlockType::Dirt, false }, -1, constants::waterLevel + 2, true);
	m_Layers.emplace_back(Block{ BlockType::Grass, false }, 0, constants::waterLevel + 2, true);
	m_Layers.emplace_back(Block{ BlockType::Sand, false }, 0, 0, true);
	addWaterLayer();

	m_Foliage.emplace_back(Foliage::FoliageType::PALM_TREE, 0.01, constants::waterLevel, constants::waterLevel + 2);
	m_Foliage.emplace_back(Foliage::FoliageType::OAK_TREE, 0.03, constants::waterLevel + 3, 255);

	setNoiseParams(7, 0.00355f);
}

OakForestBiome::~OakForestBiome() = default;

const std::vector<Layer>& OakForestBiome::getLayers() const
{
	return m_Layers;
}

const double** OakForestBiome::getHeightMap(Vector2i location)
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

const std::vector<Foliage>& OakForestBiome::getFoliage() const
{
	return m_Foliage;
}