#include <random>
#include <ctime>
#include "TerrainGenerator.h"
#include "../World/ChunkSection.h"
#include "../World/Block.h"

TerrainGenerator::TerrainGenerator()
{
	rand = std::mt19937{ static_cast<std::mt19937::result_type>(std::time(nullptr)) };
	die = std::uniform_int_distribution<>{ 0, 100 };
}

TerrainGenerator::~TerrainGenerator()
{
}

ChunkSection* TerrainGenerator::genSection(int section)
{
	ChunkSection* chunkSection{ new ChunkSection() };

	for (int x{}; x < 16; ++x)
	{
		for (int y{}; y < 16; ++y)
		{
			for (int z{}; z < 16; ++z)
			{
				if (die(rand) == 0)
					chunkSection->setBlock(Vector3i{ x, y, z }, Block{ BlockType::Grass });
				else
					chunkSection->setBlock(Vector3i{ x, y, z }, Block{ BlockType::Air });

				//chunkSection->setBlock(Vector3i{ x, y, z }, Block{ BlockType::Grass });

				/*
				if (section == 0)
					chunkSection->setBlock(Vector3i{ x, y, z }, Block{ BlockType::Grass });
				else
					chunkSection->setBlock(Vector3i{ x, y, z }, Block{ BlockType::Air });
				*/
			}
		}
	}

	return chunkSection;
}