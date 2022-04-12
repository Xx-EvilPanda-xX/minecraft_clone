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
	m_Die = std::uniform_int_distribution<>{ 0, 64 };

	std::mt19937 tempRand{ static_cast<std::mt19937::result_type>(std::time(nullptr)) };
	std::uniform_int_distribution<> tempDie{ -2147483648, 2147483647 };
	int seed{ -1852409154 };

	m_Noise.SetNoiseType(FastNoiseLite::NoiseType::NoiseType_OpenSimplex2S);
	m_Noise.SetFractalType(FastNoiseLite::FractalType::FractalType_FBm);
	m_Noise.SetFractalOctaves(5);
	m_Noise.SetFrequency(0.0075f);
	m_Noise.SetSeed(seed);

	std::cout << "Seed: " << seed << "\n";
}

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
						if (m_Die(m_Rand) < m_Die.max() / 2 && wY < constants::waterLevel - 10)
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

				if (wY == currentHeight && wY >= constants::waterLevel && m_Die(m_Rand) == 0 && trees <= m_MaxTreesPerChunk)
				{
					Tree* tree{};
					if (wY < 50)
						tree = new PalmTree{};
					else
						tree = new OakTree{};

					genTree(tree, chunkSection, pos, section);
					++trees;
					delete tree;
				}
			}
		}
	}


	return chunkSection;
}

void TerrainGenerator::genTree(Tree* tree, ChunkSection* section, Vector3i pos, const SectionLocation& treeSection)
{
	//check if tree collides with another anything (not perfect because its not possible to use getWorldBlock())
	for (int i{}; i < tree->getNumLeaves(); ++i)
	{
		const Vector3i* leaves{ tree->getLeaves() };
		Vector3i placePos{ pos.x + leaves[i].x, pos.y + leaves[i].y, pos.z + leaves[i].z };

		if (section->getBlock(placePos).getType() != BlockType::Air)
		{
			return;
		}
	}

	for (int i{}; i < tree->getTrunkHeight(); ++i)
	{
		Vector3i placePos{ pos.x, pos.y + i, pos.z };
		Block block{ tree->getTrunkType(), false};
		if (!structureShouldBeInQueue(placePos, treeSection, block))
			section->setBlock(placePos, block.getType(), false);
	}

	for (int i{}; i < tree->getNumLeaves(); ++i)
	{
		const Vector3i* leaves{ tree->getLeaves() };
		Vector3i placePos{ pos.x + leaves[i].x, pos.y + leaves[i].y, pos.z + leaves[i].z };
		Block block{ tree->getLeavesType(), false};
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