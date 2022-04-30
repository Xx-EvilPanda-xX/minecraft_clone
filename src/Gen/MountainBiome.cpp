#include "MountainBiome.h"
#include "../Constants.h"
#include "Random.h"

constexpr int ID{ 3 };

MountainBiome::MountainBiome(int seed)
	: Biome(ID, seed)
{
	m_Layers.emplace_back(BlockType::Stone, 0, true);
	m_Layers.emplace_back(BlockType::Water, constants::waterLevel, false);
	
	setNoiseParams(5, 0.00755f);
}

MountainBiome::~MountainBiome() = default;

const std::vector<Layer>& MountainBiome::getLayers() const
{
	return m_Layers;
}

const int** MountainBiome::getHeightMap(Vector2i location)
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
				height += 30.0;
				heightMap[i][j] = static_cast<int>(height);
			}
		}
	}

	return (const int**)heightMap;
}

bool MountainBiome::hasTrees() const
{
	return false;
}

bool MountainBiome::hasCactus() const
{
	return false;
}