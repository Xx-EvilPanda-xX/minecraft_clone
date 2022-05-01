#include <cmath>
#include "TerrainGenerator.h"
#include "../World/ChunkSection.h"
#include "../World/Block.h"
#include "../Constants.h"
#include "Tree.h"
#include "PalmTree.h"
#include "OakTree.h"
#include "OakForestBiome.h"
#include "DesertBiome.h"
#include "PlainsBiome.h"
#include "MountainBiome.h"

struct TreeLoc
{
	Vector3i loc;
	const Tree& tree;
};

struct TerrainMap
{
	const int** heightMap{};
	Biome* biome;
};

TerrainGenerator::TerrainGenerator(ChunkManager& manager)
	: m_Manager{ manager },
	m_Rand{ 0, 256 }
{
	Random seedRand{ -2147483648, 2147483647 };
	int seed{ seedRand.get() };
	m_Seed = seed;

	m_BiomeNoise.SetNoiseType(FastNoiseLite::NoiseType::NoiseType_OpenSimplex2S);
	m_BiomeNoise.SetFractalType(FastNoiseLite::FractalType::FractalType_FBm);
	m_BiomeNoise.SetSeed(seedRand.get());
	m_BiomeNoise.SetFractalOctaves(2);
	m_BiomeNoise.SetFrequency(0.0065f);

	std::cout << "Seed: " << seed << "\n";
}

Chunk* TerrainGenerator::generateChunk(Vector2i loc, Shader& chunkShader)
{
	const BiomeMixture** biomeMap{ getBiomeMap(loc) };
	Chunk* chunk{ new Chunk(loc, chunkShader) };
	int** heightMap{ allocHeightMap() };
	const Biome* layerMap[chunkSize][chunkSize]{};
	double bestPercentage{};
	std::vector<TerrainMap> usedMaps{};

	for (int i{}; i < chunkSize; ++i)
	{
		for (int j{}; j < chunkSize; ++j)
		{
			Biome* biomeWithMostContribution{};

			for (int k{}; k < biomeMap[i][j].getNumsElements(); ++k)
			{
				BiomeElement element{ biomeMap[i][j].getElement(k) };
				Biome* biome{ element.biome };
				const int** map{ nullptr };

				for (int l{}; l < usedMaps.size(); ++l)
				{
					if (usedMaps.at(l).biome->getId() == biome->getId())
						map = usedMaps.at(l).heightMap;
				}

				if (!map)
				{
					map = biome->getHeightMap(loc);
					usedMaps.push_back(TerrainMap{ map, biome });
				}

				if (!biomeWithMostContribution || element.percentage > bestPercentage)
				{
					bestPercentage = element.percentage;
					biomeWithMostContribution = biome;
				}

				heightMap[i][j] += static_cast<int>(static_cast<double>(map[i][j]) * element.percentage);
			}

			layerMap[i][j] = biomeWithMostContribution;
		}
	}

	for (int i{}; i < g_ChunkCap; ++i)
	{
		SectionLocation section{ i, loc };
		chunk->addSection(genSection(layerMap, (const int**)heightMap, section));
	}

	deleteHeightMap((const int**)heightMap);

	for (int i{}; i < chunkSize; ++i)
	{
		delete[] biomeMap[i];
	}

	delete[] biomeMap;

	for (int i{}; i < usedMaps.size(); ++i)
	{
		deleteHeightMap(usedMaps.at(i).heightMap);
	}

	return chunk;
}

ChunkSection* TerrainGenerator::genSection(const Biome* biomeMap[chunkSize][chunkSize], const int** heightMap, SectionLocation sectionLocation)
{
	ChunkSection* chunkSection{ new ChunkSection{} };
	
	int trees{};
	std::vector<TreeLoc> treeLocs{};

	m_Rand.setSeed(std::hash<int>{}(sectionLocation.worldPos.x) ^ std::hash<int>{}(sectionLocation.worldPos.y));

	for (int x{}; x < chunkSize; ++x)
	{
		for (int z{}; z < chunkSize; ++z)
		{
			int currentHeight{ heightMap[x][z] };
			int sectionY{ sectionLocation.sectionIndex * chunkSize };
			const Biome* biome{ biomeMap[x][z] };
			const std::vector<Layer>& biomeLayers{ biome->getLayers() };
			Layer topLayer{ biomeLayers[biomeLayers.size() - 1] };

			int y{};
			int layerIndex{};
			bool reachedTop{};
			while (y < chunkSize)
			{
				if (!reachedTop)
				{
					Layer currentLayer{ biomeLayers[layerIndex] };
					if (sectionY + y > (currentLayer.getTop() + (currentLayer.isRelative() ? currentHeight : 0)))
					{
						++layerIndex;

						if (layerIndex >= biomeLayers.size())
							reachedTop = true;

						continue;
					}
				}

				Vector3i pos{ x, y, z };
				if (!reachedTop && chunkSection->getBlock(pos).getType() == BlockType::Air)
					chunkSection->setBlock(pos, biomeLayers[layerIndex].getBlock().getType(), biomeLayers[layerIndex].getBlock().isSurface());

				++y;
			}
			
			if (!constants::flatWorld && currentHeight > sectionY && currentHeight < sectionY + 16)
				genFoliage(biome, Vector2i{ x, z }, currentHeight, chunkSection, sectionLocation, heightMap);

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

void TerrainGenerator::addHeightMaps(int** dest, const int** map1, const int** map2)
{
	for (int i{}; i < chunkSize; ++i)
	{
		for (int j{}; j < chunkSize; ++j)
		{
			dest[i][j] = map1[i][j] + map2[i][j];
		}
	}
}

void TerrainGenerator::genFoliage(const Biome* biome, Vector2i pos, int currentHeight, ChunkSection* section, const SectionLocation& sectionLocation, const int** heightMap)
{
	static const Tree& oakTree{ OakTree{} };
	static const Tree& palmTree{ PalmTree{} };

	for (int i{}; i < biome->getFoliage().size(); ++i)
	{
		Foliage foliage{ biome->getFoliage().at(i) };
		m_Rand.setRange(0, static_cast<int>(std::pow(foliage.getSpawnRate(), -1.0)));
		if (m_Rand.get() == 0 && currentHeight < foliage.getSpawnRangeTop() && currentHeight > foliage.getSpawnRangeBottom())
		{
			switch (foliage.getType())
			{
			case Foliage::FoliageType::OAK_TREE:
				genTree(oakTree, section, Vector3i{ pos.x, currentHeight - (sectionLocation.sectionIndex * 16) + 1, pos.y }, sectionLocation, heightMap);
				break;
			case Foliage::FoliageType::PALM_TREE:
				genTree(palmTree, section, Vector3i{ pos.x, currentHeight - (sectionLocation.sectionIndex * 16) + 1, pos.y }, sectionLocation, heightMap);
				break;
			case Foliage::FoliageType::CACTUS:
				genCactus(section, Vector3i{ pos.x, currentHeight - (sectionLocation.sectionIndex * 16) + 1, pos.y }, sectionLocation);
				break;
			default:
				std::cout << "Unknown foliage type.\n";
				break;
			}
		}
	}
}


void TerrainGenerator::genTree(const Tree& tree, ChunkSection* section, Vector3i pos, const SectionLocation& sectionLocation, const int** heightMap)
{
	//check if tree collides with world or another tree (not perfect because its not possible to use getWorldBlock())
	for (int i{}; i < tree.getNumLeaves(); ++i)
	{
		const Vector3i* leaves{ tree.getLeaves() };
		Vector3i placePos{ pos.x + leaves[i].x, pos.y + leaves[i].y, pos.z + leaves[i].z };
		int wY{ (sectionLocation.sectionIndex * chunkSize) + placePos.y };

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

void TerrainGenerator::genCactus(ChunkSection* section, Vector3i pos, const SectionLocation& cactusLocation)
{
	m_Rand.setRange(1, 4);
	int rand{ m_Rand.get() };
	for (int i{}; i < rand; ++i)
	{
		Vector3i placePos{ pos.x, pos.y + i, pos.z };
		Block block{ BlockType::Cactus, false };
		if (!structureShouldBeInQueue(placePos, cactusLocation, block))
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
		blockForQueuePos.x = pos.x - chunkSize;
		isOutsideSection = true;
	}
	if (pos.x < 0)
	{
		sectionForQueue.worldPos.x = section.worldPos.x - 1;
		blockForQueuePos.x = pos.x + chunkSize;
		isOutsideSection = true;
	}
	if (pos.y > 15)
	{
		sectionForQueue.sectionIndex = section.sectionIndex + 1;
		blockForQueuePos.y = pos.y - chunkSize;
		isOutsideSection = true;
		if (section.sectionIndex == 15) isOutsideWorld = true;
	}
	if (pos.y < 0)
	{
		sectionForQueue.sectionIndex = section.sectionIndex - 1;
		blockForQueuePos.y = pos.y + chunkSize;
		isOutsideSection = true;
		if (section.sectionIndex == 0) isOutsideWorld = true;
	}
	if (pos.z > 15)
	{
		sectionForQueue.worldPos.y = section.worldPos.y + 1;
		blockForQueuePos.z = pos.z - chunkSize;
		isOutsideSection = true;
	}
	if (pos.z < 0)
	{
		sectionForQueue.worldPos.y = section.worldPos.y - 1;
		blockForQueuePos.z = pos.z + chunkSize;
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

const BiomeMixture** TerrainGenerator::getBiomeMap(Vector2i location)
{
	BiomeMixture** map = new BiomeMixture*[chunkSize];

	for (int i{}; i < chunkSize; ++i)
	{
		map[i] = new BiomeMixture[chunkSize];
	}

	int chunkX{ location.x * chunkSize };
	int chunkY{ location.y * chunkSize };

	for (int i{}; i < chunkSize; ++i)
	{
		for (int j{}; j < chunkSize; ++j)
		{
			double height{ (static_cast<double>(m_BiomeNoise.GetNoise<float>(static_cast<float>(chunkX + i), static_cast<float>(chunkY + j))) / 2.0 + 0.5) * 100.0 };
			
			BiomeMixture& mixture{ map[i][j] };
			
			if (height < 100 && height > 65)
				mixture.addElement(new MountainBiome{ m_Seed }, 1.0);
			else if (height < 65 && height > 50)
				mixture.addElement(new PlainsBiome{ m_Seed }, 1.0);
			else if (height < 50 && height > 35)
			{
				//mixture.addElement(new MountainBiome{ m_Seed }, 0.10);
				mixture.addElement(new OakForestBiome{ m_Seed }, 1.0);
				//mixture.addElement(new PlainsBiome{ m_Seed }, 0.10);
				//mixture.addElement(new DesertBiome{ m_Seed }, 0.10);
			}
			else
				mixture.addElement(new DesertBiome{ m_Seed }, 1.0);
		}
	}

	return (const BiomeMixture**) map;
}

int** TerrainGenerator::allocHeightMap()
{
	int** map = new int*[chunkSize];

	for (int i{}; i < chunkSize; ++i)
	{
		map[i] = new int[chunkSize]{};
	}

	return map;
}

void TerrainGenerator::deleteHeightMap(const int** map)
{
	for (int i{}; i < chunkSize; ++i)
	{
		delete[] map[i];
	}

	delete[] map;
}

std::vector<QueueBlock>& TerrainGenerator::getBlockQueue()
{
	return m_BlockQueue;
}