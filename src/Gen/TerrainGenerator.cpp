#include <cmath>
#include "TerrainGenerator.h"
#include "../World/ChunkSection.h"
#include "../World/Block.h"
#include "../Constants.h"
#include "Tree.h"
#include "PalmTree.h"
#include "OakTree.h"

TerrainGenerator::TerrainGenerator(ChunkManager& manager)
	: m_Manager{ manager }
{
	m_Rand = std::mt19937{ static_cast<std::mt19937::result_type>(std::time(nullptr)) };
	m_Die = std::uniform_int_distribution<>{ 0, 256 };

	std::uniform_int_distribution<> tempDie{ -2147483648, 2147483647 };
	int seed{ tempDie(m_Rand) };

	m_Noise.SetNoiseType(FastNoiseLite::NoiseType::NoiseType_OpenSimplex2S);
	m_Noise.SetFractalType(FastNoiseLite::FractalType::FractalType_FBm);
	m_Noise.SetFractalOctaves(5);
	m_Noise.SetFrequency(0.0075f);
	m_Noise.SetSeed(seed);

	std::cout << "Seed: " << seed << "\n";
}

ChunkSection* TerrainGenerator::genSection(int** heightMap, SectionLocation sectionLocation)
{
	ChunkSection* chunkSection{ new ChunkSection{} };
	int trees{};
	m_Rand.seed(std::hash<int>{}(sectionLocation.worldPos.x) ^ std::hash<int>{}(sectionLocation.worldPos.y));

	for (int x{}; x < 16; ++x)
	{
		for (int y{}; y < 16; ++y)
		{
			for (int z{}; z < 16; ++z)
			{
				int wY = (sectionLocation.sectionIndex * 16) + y;
				int currentHeight{ heightMap[x][z] };
				Vector3i pos{ x, y, z };

				//terrain
				if (chunkSection->getBlock(pos).getType() == BlockType::Air)
				{
					if (wY < currentHeight - 6)
						chunkSection->setBlock(pos, BlockType::Stone, false);

					if (wY < currentHeight && wY >= currentHeight - 6)
					{
						if (wY < constants::waterLevel + 1)
						{
							if (m_Die(m_Rand) < m_Die.max() / 2 && wY < constants::waterLevel - 10)
								chunkSection->setBlock(pos, BlockType::Gravel, false);
							else
								chunkSection->setBlock(pos, BlockType::Sand, false);
						}
						else
						{
							if (wY < currentHeight - 1)
								chunkSection->setBlock(pos, BlockType::Dirt, false);
							else
								chunkSection->setBlock(pos, BlockType::Grass, false);
						}
					}

					if (wY < constants::waterLevel && wY >= currentHeight)
					{
						chunkSection->setBlock(pos, BlockType::Water, false);
						if (wY == constants::waterLevel - 1)
							chunkSection->setBlock(pos, BlockType::Water, true);
					}
				}

				//trees
				if (!constants::flatWorld && wY == currentHeight && wY >= constants::waterLevel && trees <= m_MaxTreesPerChunk)
				{
					const Tree& oakTree{ OakTree{} };
					const Tree& palmTree{ PalmTree{} };
					if (wY < 50)
					{ 
						if (m_Die(m_Rand) == 0)
						{
							genTree(palmTree, chunkSection, pos, sectionLocation, heightMap);
							++trees;
						}
					}
					else
					{
						if (m_Die(m_Rand) < 4)
						{
							genTree(oakTree, chunkSection, pos, sectionLocation, heightMap);
							++trees;
						}
					}
				}
			}
		}
	}


	return chunkSection;
}

void TerrainGenerator::genTree(const Tree& tree, ChunkSection* section, Vector3i pos, const SectionLocation& sectionLocation, int** heightMap)
{
	//check if tree collides with world or another tree (not perfect because its not possible to use getWorldBlock())
	for (int i{}; i < tree.getNumLeaves(); ++i)
	{
		const Vector3i* leaves{ tree.getLeaves() };
		Vector3i placePos{ pos.x + leaves[i].x, pos.y + leaves[i].y, pos.z + leaves[i].z };
		int wY{ placePos.y + (sectionLocation.sectionIndex * 16) };

		//tree
		if (section->getBlock(placePos).getType() != BlockType::Air)
		{
			return;
		}

		//world
		if (placePos.x <= 15 && placePos.y <= 15 && placePos.z <= 15 &&
			placePos.x >= 0 && placePos.y >= 0 && placePos.z >= 0 &&
			wY < heightMap[placePos.x][placePos.z])
		{
			return;
		}
	}

	for (int i{}; i < tree.getTrunkHeight(); ++i)
	{
		Vector3i placePos{ pos.x, pos.y + i, pos.z };
		Block block{ tree.getTrunkType(), false};
		if (!structureShouldBeInQueue(placePos, sectionLocation, block))
			section->setBlock(placePos, block.getType(), false);
	}

	for (int i{}; i < tree.getNumLeaves(); ++i)
	{
		const Vector3i* leaves{ tree.getLeaves() };
		Vector3i placePos{ pos.x + leaves[i].x, pos.y + leaves[i].y, pos.z + leaves[i].z };
		Block block{ tree.getLeavesType(), false};
		if (!structureShouldBeInQueue(placePos, sectionLocation, block))
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
				
				if (height < constants::waterLevel + 1)
				{
					height += 3;
					if (height > constants::waterLevel + 1)
						height = constants::waterLevel + 1;
				}

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