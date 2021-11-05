#include "TerrainGenerator.h"
#include "../World/ChunkSection.h"
#include "../World/Block.h"

TerrainGenerator::TerrainGenerator() = default;

TerrainGenerator::~TerrainGenerator()
{
}

ChunkSection* TerrainGenerator::genSection(int section)
{
	ChunkSection* chunkSection = new ChunkSection();

	for (int x{}; x < 16; ++x)
	{
		for (int y{}; y < 16; ++y)
		{
			for (int z{}; z < 16; ++z)
			{
				if (y < 8)
					chunkSection->setBlock(Vector3i{ x, y, z }, Block{ BlockType::Grass });
				else
					chunkSection->setBlock(Vector3i{ x, y, z }, Block{ BlockType::Air });
			}
		}
	}

	return chunkSection;
}