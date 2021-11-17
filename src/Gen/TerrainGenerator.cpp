#include <random>
#include <ctime>
#include <cmath>
#include "TerrainGenerator.h"
#include "../World/ChunkSection.h"
#include "../World/Block.h"

TerrainGenerator::TerrainGenerator()
{
	rand = std::mt19937{ static_cast<std::mt19937::result_type>(std::time(nullptr)) };
	die = std::uniform_int_distribution<>{ 11, 13 };
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

int** TerrainGenerator::getHeightMap()
{
	int** heightMap = new int*[16];

	for (int i{}; i < 16; ++i)
	{
		heightMap[i] = new int[16];
	}

	for (int i{}; i < 16; ++i)
	{
		for (int j{}; j < 16; ++j)
		{
			heightMap[i][j] = die(rand);
		}
	}

	return heightMap;
}