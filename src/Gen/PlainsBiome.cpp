#include "PlainsBiome.h"
#include "../Constants.h"
#include "Random.h"

constexpr int ID{ 2 };

PlainsBiome::PlainsBiome(int seed)
	: Biome(ID, seed)
{
	m_Layers.emplace_back(Block{ BlockType::Stone, false }, -6, 0, true);
	m_Layers.emplace_back(Block{ BlockType::Dirt, false }, -1, constants::waterLevel + 2, true);
	m_Layers.emplace_back(Block{ BlockType::Grass, false }, 0, constants::waterLevel + 2, true);
	m_Layers.emplace_back(Block{ BlockType::Sand, false }, 0, constants::waterLevel - 5, true);
	m_Layers.emplace_back(Block{ BlockType::Gravel, false }, 0, 0, true);
	m_Layers.emplace_back(Block{ BlockType::Water, false }, constants::waterLevel - 1, 0, false);
	m_Layers.emplace_back(Block{ BlockType::Water, true }, constants::waterLevel, 0, false);

	m_Foliage.emplace_back(Foliage::FoliageType::PALM_TREE, 0.01, constants::waterLevel, constants::waterLevel + 2);

	setNoiseParams(7, 0.00255f);
}

PlainsBiome::~PlainsBiome() = default;

const std::vector<Layer>& PlainsBiome::getLayers() const
{
	return m_Layers;
}

const int** PlainsBiome::getHeightMap(Vector2i location)
{
	int** heightMap = new int* [16];

	for (int i{}; i < 16; ++i)
	{
		heightMap[i] = new int[16];
	}

	int chunkX{ location.x * 16 };
	int chunkY{ location.y * 16 };

	for (int i{}; i < 16; ++i)
	{
		for (int j{}; j < 16; ++j)
		{
			if (constants::flatWorld)
			{
				heightMap[i][j] = constants::flatWorldHeight;
			}
			else
			{
				double height{ (static_cast<double>(m_Noise.GetNoise<float>(static_cast<float>(chunkX + i), static_cast<float>(chunkY + j))) / 2.0 + 0.5) * 100.0 };
				heightMap[i][j] = static_cast<int>(height);
			}
		}
	}

	return (const int**)heightMap;
}

const std::vector<Foliage>& PlainsBiome::getFoliage() const
{
	return m_Foliage;
}