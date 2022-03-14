#include <cmath>
#include "TerrainGenerator.h"
#include "../World/ChunkSection.h"
#include "../World/Block.h"
#include "../Constants.h"

TerrainGenerator::TerrainGenerator()
{
	m_Rand = std::mt19937{ static_cast<std::mt19937::result_type>(std::time(nullptr)) };
	m_Die = std::uniform_int_distribution<>{ 0, 64 };

	std::mt19937 tempRand{ static_cast<std::mt19937::result_type>(std::time(nullptr)) };
	std::uniform_int_distribution<> tempDie{ -2147483648, 2147483647 };
	int seed{ tempDie(tempRand) };

	m_Noise.SetNoiseType(FastNoiseLite::NoiseType::NoiseType_OpenSimplex2S);
	m_Noise.SetFractalType(FastNoiseLite::FractalType::FractalType_FBm);
	m_Noise.SetFractalOctaves(5);
	m_Noise.SetFrequency(0.0075f);
	m_Noise.SetSeed(seed);

	std::cout << "Seed: " << seed << "\n";
}

ChunkSection* TerrainGenerator::genSection(int** heightMap, int section)
{
	ChunkSection* chunkSection{ new ChunkSection{} };
	int trees{};

	for (int x{}; x < 16; ++x)
	{
		for (int y{}; y < 16; ++y)
		{
			for (int z{}; z < 16; ++z)
			{
				int wY = (section * 16) + y;
				int currentHeight{ heightMap[x][z] };

				if (chunkSection->getBlock(Vector3i{ x, y, z }).getType() == BlockType::Air)
				{
					if (wY < currentHeight - 6)
						chunkSection->setBlock(Vector3i{ x, y, z }, BlockType::Stone, false);
					else if (wY < currentHeight - 1 && wY >= currentHeight - 6)
						chunkSection->setBlock(Vector3i{ x, y, z }, BlockType::Dirt, false);
					else if (wY < constants::waterLevel + 1 && wY <= currentHeight)
					{
						chunkSection->setBlock(Vector3i{ x, y, z }, BlockType::Sand, false);
						if (m_Die(m_Rand) < 32 && wY < constants::waterLevel - 10)
							chunkSection->setBlock(Vector3i{ x, y, z }, BlockType::Gravel, false);
					}
					else if (wY < currentHeight && wY >= currentHeight - 1)
						chunkSection->setBlock(Vector3i{ x, y, z }, BlockType::Grass, false);
					else if (wY < constants::waterLevel)
					{
						chunkSection->setBlock(Vector3i{ x, y, z }, BlockType::Water, false);
						if (wY == constants::waterLevel - 1)
							chunkSection->setBlock(Vector3i{ x, y, z }, BlockType::Water, true);
					}
					else
						chunkSection->setBlock(Vector3i{ x, y, z }, BlockType::Air, false);
				}

				if (wY == currentHeight && wY > constants::waterLevel)
				{
					if (m_Die(m_Rand) == 0)
					{
						if (y + 4 > 15 || x >= 14 || x <= 1 || z >= 14 || z <= 1 )
							//invalid tree position
							continue;

						if (trees <= m_MaxTreesPerChunk)
						{
							genTree(chunkSection, Vector3i{ x, y, z });
							++trees;
						}
					}
				}
			}
		}
	}

	return chunkSection;
}

void TerrainGenerator::genTree(ChunkSection* section, Vector3i pos)
{
	for (int i{}; i < 4; ++i)
	{
		section->setBlock(Vector3i{ pos.x, pos.y + i, pos.z }, BlockType::Wood, false);

		if (i == 3)
		{
			section->setBlock(Vector3i{ pos.x + 1, pos.y + i, pos.z }, BlockType::Leaves, false);
			section->setBlock(Vector3i{ pos.x, pos.y + i, pos.z + 1 }, BlockType::Leaves, false);
			section->setBlock(Vector3i{ pos.x - 1, pos.y + i, pos.z }, BlockType::Leaves, false);
			section->setBlock(Vector3i{ pos.x, pos.y + i, pos.z - 1 }, BlockType::Leaves, false);

			section->setBlock(Vector3i{ pos.x + 1, pos.y + i - 1, pos.z }, BlockType::Leaves, false);
			section->setBlock(Vector3i{ pos.x, pos.y + i - 1, pos.z + 1 }, BlockType::Leaves, false);
			section->setBlock(Vector3i{ pos.x - 1, pos.y + i - 1, pos.z }, BlockType::Leaves, false);
			section->setBlock(Vector3i{ pos.x, pos.y + i - 1, pos.z - 1 }, BlockType::Leaves, false);
			section->setBlock(Vector3i{ pos.x + 1, pos.y + i - 1, pos.z + 1 }, BlockType::Leaves, false);
			section->setBlock(Vector3i{ pos.x - 1, pos.y + i - 1, pos.z + 1 }, BlockType::Leaves, false);
			section->setBlock(Vector3i{ pos.x + 1, pos.y + i - 1, pos.z - 1 }, BlockType::Leaves, false);
			section->setBlock(Vector3i{ pos.x - 1, pos.y + i - 1, pos.z - 1 }, BlockType::Leaves, false);

			section->setBlock(Vector3i{ pos.x, pos.y + i + 1,pos.z }, BlockType::Leaves, false);
		}
	}
}

int** TerrainGenerator::getHeightMap(Chunk* chunk)
{
	m_Rand.seed(std::pow(chunk->getLocation().x, chunk->getLocation().y));

	int** heightMap = new int*[16];

	for (int i{}; i < 16; ++i)
	{
		heightMap[i] = new int[16];
	}

	int chunkX{ chunk->getLocation().x * 16 };
	int chunkY{ chunk->getLocation().y * 16 };

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
				double height{ (m_Noise.GetNoise(static_cast<float>(chunkX + i), static_cast<float>(chunkY + j)) / 2.0f + 0.5f) * 100.0f };
				heightMap[i][j] = height;
			}
		}
	}

	return heightMap;
}