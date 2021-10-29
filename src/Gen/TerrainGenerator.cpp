#include "TerrainGenerator.h"
#include "../World/ChunkSection.h"
#include "../Math/Vector2i.h"
#include "../World/Block.h"

TerrainGenerator::TerrainGenerator()
{
}

TerrainGenerator::~TerrainGenerator()
{
}

ChunkSection* TerrainGenerator::genSection(Vector2i loc, int section)
{
	ChunkSection* chunkSection = new ChunkSection();

	for (int x{}; x < 16; ++x)
	{
		for (int y{}; y < 16; ++y)
		{
			for (int z{}; z < 16; ++z)
			{
				if (y < 64)
					chunkSection->setBlock(Vector3i{ x, y, z }, Block{ BlockType::Grass });
				else
					chunkSection->setBlock(Vector3i{ x, y, z }, Block{ BlockType::Air });
			}
		}
	}

	return chunkSection;
}