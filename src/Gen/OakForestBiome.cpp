#include "OakForestBiome.h"
#include "../Constants.h"

OakForestBiome::OakForestBiome()
	: Biome()
{
	m_Layers.emplace_back(BlockType::Stone, -40);
	m_Layers.emplace_back(BlockType::DiamondBlock, -30);
	m_Layers.emplace_back(BlockType::CraftingTable, -20);
	m_Layers.emplace_back(BlockType::OakLeaves, -10);
	m_Layers.emplace_back(BlockType::Dirt, -1);
	m_Layers.emplace_back(BlockType::Grass, 0);

	setNoiseParams(7, 0.00355f);
}

OakForestBiome::~OakForestBiome() = default;

const std::vector<Layer>& OakForestBiome::getLayers() const
{
	return m_Layers;
}

const int** OakForestBiome::getHeightMap(SectionLocation location)
{
	int** heightMap = new int*[16];

	for (int i{}; i < 16; ++i)
	{
		heightMap[i] = new int[16];
	}

	int chunkX{ location.worldPos.x * 16 };
	int chunkY{ location.worldPos.y * 16 };

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

				/*if (height < constants::waterLevel + 1)
				{
					height += 3;
					if (height > constants::waterLevel + 1)
						height = constants::waterLevel + 1;
				}*/

				heightMap[i][j] = static_cast<int>(height);
			}
		}
	}

	return (const int**) heightMap;
}