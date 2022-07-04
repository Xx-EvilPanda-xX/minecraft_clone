#include <cmath>
#include "TerrainGenerator.h"
#include "../World/ChunkSection.h"
#include "../World/Block.h"
#include "../Constants.h"
#include "Tree.h"
#include "PalmTree.h"
#include "OakTree.h"
#include "BushTree.h"
#include "OakForestBiome.h"
#include "DesertBiome.h"
#include "PlainsBiome.h"
#include "MountainBiome.h"
#include "GravelPlainsBiome.h"
#include "BiomeMap.h"

struct TreeLoc
{
	Vector3i loc;
	const Tree& tree;
};

struct TerrainMap
{
	double** heightMap{};
	Biome* biome;
};

TerrainGenerator::TerrainGenerator(ChunkManager& manager)
	: m_Manager{ manager },
	m_Rand{ 0, 256 }
{
	
	m_Seed = setBiomeNoiseParams(nullptr);
	std::cout << "Seed: " << m_Seed << "\n";
}

int TerrainGenerator::setBiomeNoiseParams(const int* s)
{
	Random seedRand1{ -2147483648, 2147483647 };
	Random seedRand2{ -2147483648, 2147483647 };

	int seed{ s ? *s : seedRand2.get() };

	seedRand1.setSeed(seed);

	m_BiomeNoise1.SetNoiseType(FastNoiseLite::NoiseType::NoiseType_OpenSimplex2S);
	m_BiomeNoise1.SetFractalType(FastNoiseLite::FractalType::FractalType_FBm);
	m_BiomeNoise1.SetSeed(seedRand1.get());
	m_BiomeNoise1.SetFractalOctaves(7);
	m_BiomeNoise1.SetFrequency(0.009f);

	m_BiomeNoise2.SetNoiseType(FastNoiseLite::NoiseType::NoiseType_OpenSimplex2S);
	m_BiomeNoise2.SetFractalType(FastNoiseLite::FractalType::FractalType_FBm);
	m_BiomeNoise2.SetSeed(seedRand1.get());
	m_BiomeNoise2.SetFractalOctaves(1);
	m_BiomeNoise2.SetFrequency(0.0015f);

	return seed;
}

Chunk* TerrainGenerator::generateChunk(Vector2i loc, Shader& chunkShader,
									std::pair<std::mutex&,
									std::vector<unsigned int>&> bufferDestroyQueue)
{
	BiomeMixture** biomeMap{ getBiomeMap(loc) };
	double heightMap[chunkSize][chunkSize]{};
	Biome* layerMap[chunkSize][chunkSize]{};
	
	Chunk* chunk{ new Chunk(loc, chunkShader, bufferDestroyQueue, false) };
	double bestPercentage{};
	std::vector<TerrainMap> usedMaps{};

	for (int i{}; i < chunkSize; ++i)
	{
		for (int j{}; j < chunkSize; ++j)
		{
			Biome* biomeWithMostContribution{};

			for (int k{}; k < biomeMap[i][j].getNumElements(); ++k)
			{
				BiomeElement element{ biomeMap[i][j].getElement(k) };
				Biome* biome{ element.biome };
				double** map{ nullptr };

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

				heightMap[i][j] += map[i][j] * element.percentage;
			}

			layerMap[i][j] = biomeWithMostContribution;
		}
	}

	for (int i{}; i < g_ChunkCap; ++i)
	{
		SectionLocation sectionLocation{ i, loc, chunk };
		chunk->addSection(genSection(layerMap, heightMap, sectionLocation));
	}

	deleteMap<BiomeMixture>(biomeMap);

	for (int i{}; i < usedMaps.size(); ++i)
	{
		deleteMap<double>(usedMaps.at(i).heightMap);
	}

	return chunk;
}

ChunkSection* TerrainGenerator::genSection(Biome* biomeMap[chunkSize][chunkSize],
										double heightMap[chunkSize][chunkSize],
										SectionLocation sectionLocation)
{
	ChunkSection* chunkSection{ new ChunkSection{} };
	m_Rand.setSeed(std::hash<int>{}(sectionLocation.worldPos.x) ^ std::hash<int>{}(sectionLocation.worldPos.y) ^ m_Seed);

	for (int x{}; x < chunkSize; ++x)
	{
		for (int z{}; z < chunkSize; ++z)
		{
			const double currentHeight{ heightMap[x][z] };
			const Biome* biome{ biomeMap[x][z] };
			const std::vector<Layer>& biomeLayers{ biome->getLayers() };
			
			const int sectionY{ sectionLocation.sectionIndex * chunkSize };
			int y{};
			int layerIndex{};
			bool reachedTop{};
			while (y < chunkSize)
			{
				if (!reachedTop)
				{
					Layer currentLayer{ biomeLayers[layerIndex] };
					if ((sectionY + y) > (currentLayer.getTop() + (currentLayer.isRelative() ? currentHeight : 0)))
					{
						++layerIndex;

						if (layerIndex >= biomeLayers.size())
							reachedTop = true;

						continue;
					}

					if (currentLayer.isRelative() && currentHeight < currentLayer.getVerticalLimit())
					{
						++layerIndex;

						if (layerIndex >= biomeLayers.size())
							reachedTop = true;

						continue;
					}

					Vector3i pos{ x, y, z };
					chunkSection->setBlock(pos, currentLayer.getBlock().getType(), currentLayer.getBlock().isSurface());
				}
					
				++y;
			}
			
			if (!constants::flatWorld && currentHeight > sectionY && currentHeight < (sectionY + 16))
				genFoliage(biome, Vector2i{ x, z }, chunkSection, sectionLocation, heightMap);
		}
	}

	return chunkSection;
}

void TerrainGenerator::genFoliage(const Biome* biome, Vector2i pos,
								ChunkSection* section, const SectionLocation& sectionLocation,
								double heightMap[chunkSize][chunkSize])
{
	static const Tree& oakTree{ OakTree{} };
	static const Tree& palmTree{ PalmTree{} };
	static const Tree& bushTree{ BushTree{} };

	for (int i{}; i < biome->getFoliage().size(); ++i)
	{
		Foliage foliage{ biome->getFoliage().at(i) };
		m_Rand.setRange(0, static_cast<int>(std::pow(foliage.getSpawnRate(), -1.0)));
		if (m_Rand.get() == 0 && heightMap[pos.x][pos.y] < foliage.getSpawnRangeTop() && heightMap[pos.x][pos.y] > foliage.getSpawnRangeBottom())
		{
			Vector3i placePos{ pos.x, static_cast<int>(heightMap[pos.x][pos.y]) - (sectionLocation.sectionIndex * 16) + 1, pos.y };
			switch (foliage.getType())
			{
			case Foliage::FoliageType::OAK_TREE:
				genTree(oakTree, section, placePos, sectionLocation, heightMap);
				break;
			case Foliage::FoliageType::PALM_TREE:
				genTree(palmTree, section, placePos, sectionLocation, heightMap);
				break;
			case Foliage::FoliageType::BUSH:
				genTree(bushTree, section, placePos, sectionLocation, heightMap);
				break;
			case Foliage::FoliageType::CACTUS:
				genCactus(section, placePos, sectionLocation);
				break;
			case Foliage::FoliageType::TALL_GRASS:
				if (section->getBlock(placePos).getType() == BlockType::Air)
					section->setBlock(placePos, BlockType::TallGrass, false);
				break;
			case Foliage::FoliageType::SHRUB:
				if (section->getBlock(placePos).getType() == BlockType::Air)
					section->setBlock(placePos, BlockType::Shrub, false);
				break;
			case Foliage::FoliageType::ROSE:
				if (section->getBlock(placePos).getType() == BlockType::Air)
					section->setBlock(placePos, BlockType::Rose, false);
				break;
			default:
				std::cout << "Undefined foliage type.\n";
				break;
			}
		}
	}
}


void TerrainGenerator::genTree(const Tree& tree, ChunkSection* section,
							Vector3i pos, const SectionLocation& sectionLocation,
							double heightMap[chunkSize][chunkSize])
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

void TerrainGenerator::genCactus(ChunkSection* section, Vector3i pos, const SectionLocation& sectionLocation)
{
	m_Rand.setRange(1, 4);
	int rand{ m_Rand.get() };
	for (int i{}; i < rand; ++i)
	{
		Vector3i placePos{ pos.x, pos.y + i, pos.z };
		Block block{ BlockType::Cactus, false };
		if (!structureShouldBeInQueue(placePos, sectionLocation, block))
			section->setBlock(placePos, block.getType(), false);
	}
}

bool TerrainGenerator::structureShouldBeInQueue(Vector3i pos, const SectionLocation& sectionLocation, Block block)
{
	SectionLocation sectionForQueue{ sectionLocation.sectionIndex, sectionLocation.worldPos, nullptr };
	Vector3i blockForQueuePos{ pos };
	bool isOutsideSection{ false };
	bool isOutsideWorld{ false };

	if (pos.x > 15)
	{
		sectionForQueue.worldPos.x = sectionLocation.worldPos.x + 1;
		blockForQueuePos.x = pos.x - chunkSize;
		isOutsideSection = true;
	}
	if (pos.x < 0)
	{
		sectionForQueue.worldPos.x = sectionLocation.worldPos.x - 1;
		blockForQueuePos.x = pos.x + chunkSize;
		isOutsideSection = true;
	}
	if (pos.y > 15)
	{
		sectionForQueue.sectionIndex = sectionLocation.sectionIndex + 1;
		blockForQueuePos.y = pos.y - chunkSize;
		isOutsideSection = true;
		if (sectionLocation.sectionIndex == 15) isOutsideWorld = true;
	}
	if (pos.y < 0)
	{
		sectionForQueue.sectionIndex = sectionLocation.sectionIndex - 1;
		blockForQueuePos.y = pos.y + chunkSize;
		isOutsideSection = true;
		if (sectionLocation.sectionIndex == 0) isOutsideWorld = true;
	}
	if (pos.z > 15)
	{
		sectionForQueue.worldPos.y = sectionLocation.worldPos.y + 1;
		blockForQueuePos.z = pos.z - chunkSize;
		isOutsideSection = true;
	}
	if (pos.z < 0)
	{
		sectionForQueue.worldPos.y = sectionLocation.worldPos.y - 1;
		blockForQueuePos.z = pos.z + chunkSize;
		isOutsideSection = true;
	}

	if (isOutsideSection && !isOutsideWorld)
	{
		QueueBlock queueBlock{ block,
							blockForQueuePos,
							sectionForQueue };

		m_BlockQueue.push_back(queueBlock);
		sectionLocation.containingChunk->addQueueBlock(queueBlock);
	}

	return isOutsideSection;
}

BiomeMixture** TerrainGenerator::getBiomeMap(Vector2i location)
{
	BiomeMixture** map{ allocMap<BiomeMixture>() };

	int chunkX{ location.x * chunkSize };
	int chunkY{ location.y * chunkSize };

	for (int i{}; i < chunkSize; ++i)
	{
		for (int j{}; j < chunkSize; ++j)
		{
			double height1{ (m_BiomeNoise1.GetNoise<float>(static_cast<float>(chunkX + i), static_cast<float>(chunkY + j)) / 2.0 + 0.5) * 250.0 };
			double height2{ m_BiomeNoise2.GetNoise<float>(static_cast<float>(chunkX + i), static_cast<float>(chunkY + j)) * 90.0 };
			double height = height1 + height2;

			if (height > 255)
			{
				height = 255;
				//std::cout << "ERR: Height map returned a value greater than 255!\nClamping to 255!\n";
			}
				
			if (height < 0)
			{
				height = 0;
				//std::cout << "ERR: Height map returned a value less than 0!\nClamping to 0!\n";
			}

			BiomeMixture& mixture{ map[i][j] };
			
			if (constants::flatWorld)
			{
				mixture.addElement(new PlainsBiome{ m_Seed }, 1.0);
				continue;
			}

			//mountains
			if (height <= Biomes::s_Mountains.top && height >= Biomes::s_Mountains.bottom)
				mixture.addElement(new MountainBiome{ m_Seed }, 1.0);

			//transition between mountains and gravel plains
			else if (height <= Biomes::s_Mountains.bottom && height >= Biomes::s_GravelPlains.top)
			{
				double div{ static_cast<double>(Biomes::s_Mountains.bottom - Biomes::s_GravelPlains.top) };
				mixture.addElement(new MountainBiome{ m_Seed }, std::abs(Biomes::s_GravelPlains.top - height) / div);
				mixture.addElement(new GravelPlainsBiome{ m_Seed }, std::abs(Biomes::s_Mountains.bottom - height) / div);
			}

			//gravel plains
			else if (height <= Biomes::s_GravelPlains.top && height >= Biomes::s_GravelPlains.bottom)
				mixture.addElement(new GravelPlainsBiome{ m_Seed }, 1.0);

			//oak forest (no transition since heightmaps are the same)
			else if (height <= Biomes::s_OakForest.top && height >= Biomes::s_OakForest.bottom)
				mixture.addElement(new OakForestBiome{ m_Seed }, 1.0);

			//transition between oak forest and plains
			else if (height <= Biomes::s_OakForest.bottom && height >= Biomes::s_Plains.top)
			{
				double div{ static_cast<double>(Biomes::s_OakForest.bottom - Biomes::s_Plains.top) };
				mixture.addElement(new OakForestBiome{ m_Seed }, std::abs(Biomes::s_Plains.top - height) / div);
				mixture.addElement(new PlainsBiome{ m_Seed }, std::abs(Biomes::s_OakForest.bottom - height) / div);
			}

			//plains
			else if (height <= Biomes::s_Plains.top && height >= Biomes::s_Plains.bottom)
				mixture.addElement(new PlainsBiome{ m_Seed }, 1.0);

			//desert (no transition since heightmaps are the same)
			else if (height <= Biomes::s_Desert.top && height >= Biomes::s_Desert.bottom)
				mixture.addElement(new DesertBiome{ m_Seed }, 1.0);
		}
	}

	return map;
}

template <typename T>
T** TerrainGenerator::allocMap()
{
	T** map = new T*[chunkSize];

	for (int i{}; i < chunkSize; ++i)
	{
		map[i] = new T[chunkSize]{};
	}

	return map;
}

template <typename T>
void TerrainGenerator::deleteMap(T** map)
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

int TerrainGenerator::getSeed() const
{
	return m_Seed;
}

void TerrainGenerator::setSeed(int newSeed)
{
	m_Seed = setBiomeNoiseParams(&newSeed);
}