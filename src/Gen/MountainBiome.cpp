#include "MountainBiome.h"
#include "../Constants.h"
#include "Random.h"

constexpr int ID{ 3 };

MountainBiome::MountainBiome(int seed)
	: Biome(ID, seed)
{
	m_Layers.emplace_back(Block{ BlockType::Stone, false }, -3, 0, true);
	m_Layers.emplace_back(Block{ BlockType::Snow, false }, 0, 85, true);
	m_Layers.emplace_back(Block{ BlockType::Stone, false }, 0, 0, true);
	addWaterLayer();

	m_Foliage.emplace_back(Foliage::FoliageType::OAK_TREE, 0.0075, constants::waterLevel, 85);
	
	setNoiseParams(7, 0.008f);
}

MountainBiome::~MountainBiome() = default;

const std::vector<Layer>& MountainBiome::getLayers() const
{
	return m_Layers;
}

const double** MountainBiome::getHeightMap(Vector2i location)
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

			heightMap[i][j] += 35.0;

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

const std::vector<Foliage>& MountainBiome::getFoliage() const
{
	return m_Foliage;
}