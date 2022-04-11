#include <cmath>
#include "TerrainGenerator.h"
#include "../World/ChunkSection.h"
#include "../World/Block.h"
#include "../Constants.h"

TerrainGenerator::TerrainGenerator(ChunkManager& manager)
	: m_Manager{ manager }
{
	m_Rand = std::mt19937{ static_cast<std::mt19937::result_type>(std::time(nullptr)) };
	m_Die = std::uniform_int_distribution<>{ 0, 32 };

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

Vector3i TerrainGenerator::treeLeaves[13]{ Vector3i{ 1, 3, 0 }, Vector3i{ 0, 3, 1 }, Vector3i{ -1, 3, 0 }, Vector3i{ 0, 3, -1 }, Vector3i{ 1, 2, 0 }, 
										   Vector3i{ 0, 2, 1 }, Vector3i{ -1, 2, 0 }, Vector3i{ 0, 2, -1 }, Vector3i{ 1, 2, 1 }, Vector3i{ -1, 2, 1 }, 
										   Vector3i{ 1, 2, -1 }, Vector3i{ -1, 2, -1 }, Vector3i{ 0, 4, 0} };

ChunkSection* TerrainGenerator::genSection(int** heightMap, SectionLocation section)
{
	ChunkSection* chunkSection{ new ChunkSection{} };
	int trees{};

	for (int x{}; x < 16; ++x)
	{
		for (int y{}; y < 16; ++y)
		{
			for (int z{}; z < 16; ++z)
			{
				int wY = (section.sectionIndex * 16) + y;
				int currentHeight{ heightMap[x][z] };
				Vector3i pos{ x, y, z };

				if (chunkSection->getBlock(pos).getType() == BlockType::Air)
				{
					if (wY < currentHeight - 6)
						chunkSection->setBlock(pos, BlockType::Stone, false);
					else if (wY < currentHeight - 1 && wY >= currentHeight - 6)
						chunkSection->setBlock(pos, BlockType::Dirt, false);
					else if (wY < constants::waterLevel + 1 && wY <= currentHeight)
					{
						chunkSection->setBlock(pos, BlockType::Sand, false);
						if (m_Die(m_Rand) < 32 && wY < constants::waterLevel - 10)
							chunkSection->setBlock(pos, BlockType::Gravel, false);
					}
					else if (wY < currentHeight && wY >= currentHeight - 1)
						chunkSection->setBlock(pos, BlockType::Grass, false);
					else if (wY < constants::waterLevel)
					{
						chunkSection->setBlock(pos, BlockType::Water, false);
						if (wY == constants::waterLevel - 1)
							chunkSection->setBlock(pos, BlockType::Water, true);
					}
					else
						chunkSection->setBlock(pos, BlockType::Air, false);

				}

				if (wY == currentHeight && wY > constants::waterLevel)
				{
					if (m_Die(m_Rand) == 0)
					{
						//if (y + 4 > 15 || x >= 14 || x <= 1 || z >= 14 || z <= 1 )
							//invalid tree position
							//continue;

						if (trees <= m_MaxTreesPerChunk)
						{
							genTree(chunkSection, pos, section);
							++trees;
						}
					}
				}
			}
		}
	}


	return chunkSection;
}

void TerrainGenerator::genTree(ChunkSection* section, Vector3i pos, const SectionLocation& treeSection)
{
	for (int i{}; i < 4; ++i)
	{
		Vector3i placePos{ pos.x, pos.y + i, pos.z };
		Block block{ BlockType::Wood, false };
		if (!structureShouldBeInQueue(placePos, treeSection, block))
			section->setBlock(placePos, block.getType(), false);
	}

	for (int i{}; i < 13; ++i)
	{
		Vector3i placePos{ pos.x + treeLeaves[i].x, pos.y + treeLeaves[i].y, pos.z + treeLeaves[i].z };
		Block block{ BlockType::Leaves, false };
		if (!structureShouldBeInQueue(placePos, treeSection, block))
			section->setBlock(placePos, block.getType(), false);
	}
}

bool TerrainGenerator::structureShouldBeInQueue(Vector3i pos, const SectionLocation& section, Block block)
{
	SectionLocation sectionForQueue{ section };
	Vector3i blockForQueuePos{ pos };
	bool isOutsideSection{ false };
	bool isOutsideWorld{ false };

	if (pos.x > 15)
	{
		sectionForQueue.worldPos.x = section.worldPos.x + 1;
		blockForQueuePos.x = pos.x - 16;
		isOutsideSection = true;
	}
	if (pos.x < 0)
	{
		sectionForQueue.worldPos.x = section.worldPos.x - 1;
		blockForQueuePos.x = pos.x + 16;
		isOutsideSection = true;
	}
	if (pos.y > 15)
	{
		sectionForQueue.sectionIndex = section.sectionIndex + 1;
		blockForQueuePos.y = pos.y - 16;
		isOutsideSection = true;
		if (section.sectionIndex == 15) isOutsideWorld = true;
	}
	if (pos.y < 0)
	{
		sectionForQueue.sectionIndex = section.sectionIndex - 1;
		blockForQueuePos.y = pos.y + 16;
		isOutsideSection = true;
		if (section.sectionIndex == 0) isOutsideWorld = true;
	}
	if (pos.z > 15)
	{
		sectionForQueue.worldPos.y = section.worldPos.y + 1;
		blockForQueuePos.z = pos.z - 16;
		isOutsideSection = true;
	}
	if (pos.z < 0)
	{
		sectionForQueue.worldPos.y = section.worldPos.y - 1;
		blockForQueuePos.z = pos.z + 16;
		isOutsideSection = true;
	}

	if (isOutsideSection && !isOutsideWorld)
	{
		m_BlockQueue.push_back(QueueBlock{ block,
										   blockForQueuePos,
										   sectionForQueue });
	}

	return isOutsideSection;
}

int** TerrainGenerator::getHeightMap(Chunk* chunk)
{
	m_Rand.seed(static_cast<unsigned int>(std::pow(chunk->getLocation().x, chunk->getLocation().y)));

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
				double height{ (static_cast<double>(m_Noise.GetNoise(static_cast<float>(chunkX + i), static_cast<float>(chunkY + j))) / 2.0 + 0.5) * 100.0 };
				heightMap[i][j] = static_cast<int>(height);
			}
		}
	}

	return heightMap;
}

std::vector<QueueBlock>& TerrainGenerator::getBlockQueue()
{
	return m_BlockQueue;
}