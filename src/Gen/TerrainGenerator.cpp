#include <cmath>
#include "TerrainGenerator.h"
#include "../World/ChunkSection.h"
#include "../World/Block.h"

TerrainGenerator::TerrainGenerator()
{
	m_Noise.SetNoiseType(FastNoiseLite::NoiseType::NoiseType_Perlin);
	m_Noise.SetFrequency(0.015f);
}

ChunkSection* TerrainGenerator::genSection(int** heightMap, int section)
{
	ChunkSection* chunkSection{ new ChunkSection() };

	for (int x{}; x < 16; ++x)
	{
		for (int y{}; y < 16; ++y)
		{
			for (int z{}; z < 16; ++z)
			{
				int wY = (section * 16) + y;
				int currentHeight{ heightMap[x][z] };
				if (wY < currentHeight - 6)
					chunkSection->setBlock(Vector3i{ x, y, z }, Block{ BlockType::Stone });
				else if (wY < currentHeight - 1 && wY >= currentHeight - 6)
					chunkSection->setBlock(Vector3i{ x, y, z }, Block{ BlockType::Dirt });
				else if (wY < currentHeight && wY >= currentHeight - 1)
					chunkSection->setBlock(Vector3i{ x, y, z }, Block{ BlockType::Grass });
				else
					chunkSection->setBlock(Vector3i{ x, y, z }, Block{ BlockType::Air });
			}
		}
	}

	return chunkSection;
}

int** TerrainGenerator::getHeightMap(Chunk* chunk)
{
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
			float height{ (m_Noise.GetNoise(static_cast<float>(chunkX + i), static_cast<float>(chunkY + j)) / 2.0f + 0.5f) * 100.0f};
			heightMap[i][j] = height;
		}
	}

	return heightMap;
}