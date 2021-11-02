#include "TerrainGenerator.h"
#include "../World/ChunkSection.h"
#include <glm/vec2.hpp>
#include "../World/Block.h"

TerrainGenerator::TerrainGenerator()
{
}

TerrainGenerator::~TerrainGenerator()
{
}

ChunkSection* TerrainGenerator::genSection(glm::vec2  loc, int section)
{
	ChunkSection* chunkSection = new ChunkSection();

	for (int x{}; x < 16; ++x)
	{
		for (int y{}; y < 16; ++y)
		{
			for (int z{}; z < 16; ++z)
			{
				int chunkY = loc.y + (section * 16);
				if (chunkY < 64)
					chunkSection->setBlock(glm::vec3{ x, y, z }, Block{ BlockType::Grass });
				else
					chunkSection->setBlock(glm::vec3{ x, y, z }, Block{ BlockType::Air });
			}
		}
	}

	return chunkSection;
}