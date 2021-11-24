#include <cmath>
#include "TerrainGenerator.h"
#include "../World/ChunkSection.h"
#include "../World/Block.h"
#include "../Constants.h"

TerrainGenerator::TerrainGenerator()
{
	rand = std::mt19937{ static_cast<std::mt19937::result_type>(std::time(nullptr)) };
	die = std::uniform_int_distribution<>{ 0, 64 };

	m_Noise.SetNoiseType(FastNoiseLite::NoiseType::NoiseType_OpenSimplex2S);
	m_Noise.SetFractalType(FastNoiseLite::FractalType::FractalType_FBm);
	m_Noise.SetFractalOctaves(7);
	m_Noise.SetFrequency(0.005f);
}

ChunkSection* TerrainGenerator::genSection(int** heightMap, int section)
{
	ChunkSection* chunkSection{ new ChunkSection{} };

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
						chunkSection->setBlock(Vector3i{ x, y, z }, BlockType::Stone);
					else if (wY < currentHeight - 1 && wY >= currentHeight - 6)
						chunkSection->setBlock(Vector3i{ x, y, z }, BlockType::Dirt);
					else if (wY < currentHeight && wY >= currentHeight - 1)
						chunkSection->setBlock(Vector3i{ x, y, z }, BlockType::Grass);
					else if (wY < constants::waterLevel)
						chunkSection->setBlock(Vector3i{ x, y, z }, BlockType::Water);
					else
						chunkSection->setBlock(Vector3i{ x, y, z }, BlockType::Air);
				}

				if (wY == currentHeight && wY > constants::waterLevel)
				{
					if (die(rand) == 0)
					{
						if (y + 4 > 15 || x >= 14 || x <= 1 || z >= 14 || z <= 1 )
							//invalid tree position
							continue;

						genTree(chunkSection, Vector3i{ x, y, z } );
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
		section->setBlock(Vector3i{ pos.x, pos.y + i, pos.z }, BlockType::Wood);

		if (i == 3)
		{
			section->setBlock(Vector3i{ pos.x + 1, pos.y + i, pos.z }, BlockType::Leaves);
			section->setBlock(Vector3i{ pos.x, pos.y + i, pos.z + 1 }, BlockType::Leaves);
			section->setBlock(Vector3i{ pos.x - 1, pos.y + i, pos.z }, BlockType::Leaves);
			section->setBlock(Vector3i{ pos.x, pos.y + i, pos.z - 1 }, BlockType::Leaves);

			section->setBlock(Vector3i{ pos.x + 1, pos.y + i - 1, pos.z }, BlockType::Leaves);
			section->setBlock(Vector3i{ pos.x, pos.y + i - 1, pos.z + 1 }, BlockType::Leaves);
			section->setBlock(Vector3i{ pos.x - 1, pos.y + i - 1, pos.z }, BlockType::Leaves);
			section->setBlock(Vector3i{ pos.x, pos.y + i - 1, pos.z - 1 }, BlockType::Leaves);
			section->setBlock(Vector3i{ pos.x + 1, pos.y + i - 1, pos.z + 1 }, BlockType::Leaves);
			section->setBlock(Vector3i{ pos.x - 1, pos.y + i - 1, pos.z + 1 }, BlockType::Leaves);
			section->setBlock(Vector3i{ pos.x + 1, pos.y + i - 1, pos.z - 1 }, BlockType::Leaves);
			section->setBlock(Vector3i{ pos.x - 1, pos.y + i - 1, pos.z - 1 }, BlockType::Leaves);

			section->setBlock(Vector3i{ pos.x, pos.y + i + 1,pos.z }, BlockType::Leaves);
		}
	}
}

int** TerrainGenerator::getHeightMap(Chunk* chunk)
{
	rand.seed(std::pow(chunk->getLocation().x, chunk->getLocation().y));

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
				float height{ (m_Noise.GetNoise(static_cast<float>(chunkX + i), static_cast<float>(chunkY + j)) / 2.0f + 0.5f) * 100.0f };
				heightMap[i][j] = height;
			}
		}
	}

	return heightMap;
}