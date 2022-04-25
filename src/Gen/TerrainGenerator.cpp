#include <cmath>
#include "TerrainGenerator.h"
#include "../World/ChunkSection.h"
#include "../World/Block.h"
#include "../Constants.h"
#include "Tree.h"
#include "PalmTree.h"
#include "OakTree.h"
#include "OakForestBiome.h"

struct TreeLoc
{
	Vector3i loc;
	const Tree& tree;
};

TerrainGenerator::TerrainGenerator(ChunkManager& manager)
	: m_Manager{ manager }
{
	m_Rand = std::mt19937{ static_cast<std::mt19937::result_type>(std::time(nullptr)) };
	m_Die = std::uniform_int_distribution<>{ 0, 256 };

	std::uniform_int_distribution<> tempDie{ -2147483648, 2147483647 };
	int seed{ tempDie(m_Rand) };

	std::cout << "Seed: " << seed << "\n";
}

Chunk* TerrainGenerator::generateChunk(Vector2i loc, Shader& chunkShader)
{
	static Biome&& biome{ OakForestBiome{} };
	Chunk* chunk{ new Chunk(loc, chunkShader) };
	const int** heightMap{ biome.getHeightMap(SectionLocation{ 0, loc })};

	for (int i{}; i < g_ChunkCap; ++i)
	{
		SectionLocation section{ i, loc };
		chunk->addSection(genSection(biome.getLayers(), heightMap, section));
	}

	for (int i{}; i < 16; ++i)
	{
		delete[] heightMap[i];
	}

	delete[] heightMap;

	return chunk;
}

//TODO: add biome map
ChunkSection* TerrainGenerator::genSection(const std::vector<Layer>& biomeLayers, const int** heightMap, SectionLocation sectionLocation)
{
	static const Tree& oakTree{ OakTree{} };
	static const Tree& palmTree{ PalmTree{} };
	
	ChunkSection* chunkSection{ new ChunkSection{} };
	
	int trees{};
	std::vector<TreeLoc> treeLocs{};

	m_Rand.seed(std::hash<int>{}(sectionLocation.worldPos.x) ^ std::hash<int>{}(sectionLocation.worldPos.y));

	for (int x{}; x < 16; ++x)
	{
		for (int z{}; z < 16; ++z)
		{
			int currentHeight{ heightMap[x][z] };
			int sectionY{ sectionLocation.sectionIndex * 16 };

			if (sectionY > (biomeLayers[biomeLayers.size() - 1].getTop() + currentHeight))
				continue;

			int y{};
			int layerIndex{};
			bool reachedTop{};
			while (y < 16)
			{
				if (!reachedTop)
				{
					if ((sectionLocation.sectionIndex * 16) + y > (biomeLayers[layerIndex].getTop() + currentHeight))
					{
						++layerIndex;

						if (layerIndex >= biomeLayers.size())
							reachedTop = true;

						continue;
					}
				}

				Vector3i pos{ x, y, z };
				if (!reachedTop && chunkSection->getBlock(pos).getType() == BlockType::Air)
					chunkSection->setBlock(pos, biomeLayers[layerIndex].getType(), false);

				++y;
			}
			
			if (!constants::flatWorld && currentHeight > sectionY && currentHeight < sectionY + 16 /*&& currentHeight >= constants::waterLevel*/ && trees <= m_MaxTreesPerChunk)
			{
				if (currentHeight < 50 && currentHeight > 46)
				{
					if (m_Die(m_Rand) == 0)
					{
						treeLocs.push_back({ { x, currentHeight - (sectionLocation.sectionIndex * 16) + 1, z }, palmTree });
						++trees;
					}
				}
				else
				{
					if (m_Die(m_Rand) < 4)
					{
						treeLocs.push_back({ { x, currentHeight - (sectionLocation.sectionIndex * 16) + 1, z }, oakTree });
						++trees;
					}
				}
			}

			//for (int y{}; y < 16; ++y)
			//{//
				//int wY = (sectionLocation.sectionIndex * 16) + y;
				
				//BlockType block{ getBlockFromLayer(biome, wY, currentHeight) };
				

				////terrain
				//if (chunkSection->getBlock(pos).getType() == BlockType::Air)
				//{
				//	if (wY < currentHeight - 6)
				//		chunkSection->setBlock(pos, BlockType::Stone, false);

				//	if (wY < currentHeight && wY >= currentHeight - 6)
				//	{
				//		if (wY < constants::waterLevel + 1)
				//		{
				//			if (m_Die(m_Rand) < m_Die.max() / 2 && wY < constants::waterLevel - 10)
				//				chunkSection->setBlock(pos, BlockType::Gravel, false);
				//			else
				//				chunkSection->setBlock(pos, BlockType::Sand, false);
				//		}
				//		else
				//		{
				//			if (wY < currentHeight - 1)
				//				chunkSection->setBlock(pos, BlockType::Dirt, false);
				//			else
				//				chunkSection->setBlock(pos, BlockType::Grass, false);
				//		}
				//	}

				//	if (wY < constants::waterLevel && wY >= currentHeight)
				//	{
				//		chunkSection->setBlock(pos, BlockType::Water, false);
				//		if (wY == constants::waterLevel - 1)
				//			chunkSection->setBlock(pos, BlockType::Water, true);
				//	}
				//}
			//}
		}
	}

	for (const TreeLoc& pos : treeLocs)
	{
		genTree(pos.tree, chunkSection, pos.loc, sectionLocation, heightMap);
	}

	return chunkSection;
}

void TerrainGenerator::genTree(const Tree& tree, ChunkSection* section, Vector3i pos, const SectionLocation& sectionLocation, const int** heightMap)
{
	//check if tree collides with world or another tree (not perfect because its not possible to use getWorldBlock())
	for (int i{}; i < tree.getNumLeaves(); ++i)
	{
		const Vector3i* leaves{ tree.getLeaves() };
		Vector3i placePos{ pos.x + leaves[i].x, pos.y + leaves[i].y, pos.z + leaves[i].z };
		int wY{ (sectionLocation.sectionIndex * 16) + placePos.y };

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

std::vector<QueueBlock>& TerrainGenerator::getBlockQueue()
{
	return m_BlockQueue;
}