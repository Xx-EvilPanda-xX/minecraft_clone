#include <random>
#include <ctime>
#include <cmath>
#include "TerrainGenerator.h"
#include "../World/ChunkSection.h"
#include "../World/Block.h"

TerrainGenerator::TerrainGenerator()
{
	rand = std::mt19937{ static_cast<std::mt19937::result_type>(std::time(nullptr)) };
	die = std::uniform_int_distribution<>{ 16, 18 };
}

ChunkSection* TerrainGenerator::genSection(int section)
{
	ChunkSection* chunkSection{ new ChunkSection() };

	double hieghtMap[16][16];

	for (int i{}; i < 16; ++i)
	{
		for (int j{}; j < 16; ++j)
		{
			//hieghtMap[i][j] = std::sin(i) * 16;
			hieghtMap[i][j] = die(rand);
		}
	}

	for (int x{}; x < 16; ++x)
	{
		for (int y{}; y < 16; ++y)
		{
			for (int z{}; z < 16; ++z)
			{
				int wY = (section * 16) + y;

				//if (die(rand) == 0)
				//	chunkSection->setBlock(Vector3i{ x, y, z }, Block{ BlockType::Grass });
				//else
				//	chunkSection->setBlock(Vector3i{ x, y, z }, Block{ BlockType::Air });

				//chunkSection->setBlock(Vector3i{ x, y, z }, Block{ BlockType::Grass });

				if (wY < hieghtMap[x][z])
					chunkSection->setBlock(Vector3i{ x, y, z }, Block{ BlockType::Grass });
				else
					chunkSection->setBlock(Vector3i{ x, y, z }, Block{ BlockType::Air });
				
			}
		}
	}

	return chunkSection;
}