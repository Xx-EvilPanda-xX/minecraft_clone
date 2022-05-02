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
#include "BiomeMap.h"

struct TreeLoc
{
	Vector3i loc;
	const Tree& tree;
};

struct TerrainMap
{
	const double** heightMap{};
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
	m_BiomeNoise.SetFractalOctaves(6);
	m_BiomeNoise.SetFrequency(0.0065f);

	std::cout << "Seed: " << seed << "\n";
}

Chunk* TerrainGenerator::generateChunk(Vector2i loc, Shader& chunkShader)
{
	const BiomeMixture** biomeMap{ getBiomeMap(loc) };
	double** heightMap{ allocMap<double>() };
	const Biome* layerMap[chunkSize][chunkSize]{};
	
	Chunk* chunk{ new Chunk(loc, chunkShader) };
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
				const double** map{ nullptr };

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

				heightMap[i][j] += static_cast<double>(map[i][j]) * element.percentage;
			}

			layerMap[i][j] = biomeWithMostContribution;
		}
	}

	for (int i{}; i < g_ChunkCap; ++i)
	{
		SectionLocation section{ i, loc };
		chunk->addSection(genSection(layerMap, (const double**) heightMap, section));
	}

	deleteMap<double>((const double**)heightMap);
	deleteMap<BiomeMixture>(biomeMap);

	for (int i{}; i < usedMaps.size(); ++i)
	{
		deleteMap<double>(usedMaps.at(i).heightMap);
	}

	return chunk;
}

ChunkSection* TerrainGenerator::genSection(const Biome* biomeMap[chunkSize][chunkSize], const double** heightMap, SectionLocation sectionLocation)
{
	ChunkSection* chunkSection{ new ChunkSection{} };
	m_Rand.setSeed(std::hash<int>{}(sectionLocation.worldPos.x) ^ std::hash<int>{}(sectionLocation.worldPos.y));

	for (int x{}; x < chunkSize; ++x)
	{
		for (int z{}; z < chunkSize; ++z)
		{
			double currentHeight{ heightMap[x][z] };
			const Biome* biome{ biomeMap[x][z] };
			const std::vector<Layer>& biomeLayers{ biome->getLayers() };
			Layer topLayer{ biomeLayers[biomeLayers.size() - 1] };
			
			int sectionY{ sectionLocation.sectionIndex * chunkSize };
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
						continue;
					}

					Vector3i pos{ x, y, z };
					chunkSection->setBlock(pos, currentLayer.getBlock().getType(), currentLayer.getBlock().isSurface());
				}
					
				++y;
			}
			
			if (!constants::flatWorld && currentHeight > sectionY && currentHeight < (sectionY + 16))
				genFoliage(biome, Vector2i{ x, z }, currentHeight, chunkSection, sectionLocation, heightMap);
		}
	}

	return chunkSection;
}

void TerrainGenerator::genFoliage(const Biome* biome, Vector2i pos, double currentHeight, ChunkSection* section, const SectionLocation& sectionLocation, const double** heightMap)
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
				genTree(oakTree, section, Vector3i{ pos.x, static_cast<int>(currentHeight) - (sectionLocation.sectionIndex * 16) + 1, pos.y }, sectionLocation, heightMap);
				break;
			case Foliage::FoliageType::PALM_TREE:
				genTree(palmTree, section, Vector3i{ pos.x, static_cast<int>(currentHeight) - (sectionLocation.sectionIndex * 16) + 1, pos.y }, sectionLocation, heightMap);
				break;
			case Foliage::FoliageType::CACTUS:
				genCactus(section, Vector3i{ pos.x, static_cast<int>(currentHeight) - (sectionLocation.sectionIndex * 16) + 1, pos.y }, sectionLocation);
				break;
			default:
				std::cout << "Unknown foliage type.\n";
				break;
			}
		}
	}
}


void TerrainGenerator::genTree(const Tree& tree, ChunkSection* section, Vector3i pos, const SectionLocation& sectionLocation, const double** heightMap)
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
	BiomeMixture** map{ allocMap<BiomeMixture>() };

	int chunkX{ location.x * chunkSize };
	int chunkY{ location.y * chunkSize };

	for (int i{}; i < chunkSize; ++i)
	{
		for (int j{}; j < chunkSize; ++j)
		{
			double height{ (m_BiomeNoise.GetNoise<float>(static_cast<float>(chunkX + i), static_cast<float>(chunkY + j)) / 2.0 + 0.5) * 200.0 };
			
			if (height > 255)
				height = 255;
			if (height < 0)
				height = 0;

			BiomeMixture& mixture{ map[i][j] };
			
			//mountains
			if (height <= Biomes::s_Mountains.top && height >= Biomes::s_Mountains.bottom)
				mixture.addElement(new MountainBiome{ m_Seed }, 1.0);

			//transition between mountains and oak forest
			else if (height <= Biomes::s_Mountains.bottom && height >= Biomes::s_OakForest.top)
			{
				double div{ static_cast<double>(Biomes::s_Mountains.bottom - Biomes::s_OakForest.top) };
				mixture.addElement(new OakForestBiome{ m_Seed }, std::abs(Biomes::s_Mountains.bottom - height) / div);
				mixture.addElement(new MountainBiome{ m_Seed }, std::abs(Biomes::s_OakForest.top - height) / div);
			}

			//oak forest
			else if (height <= Biomes::s_OakForest.top && height >= Biomes::s_OakForest.bottom)
				mixture.addElement(new OakForestBiome{ m_Seed }, 1.0);

			//plains (no transition since heightmaps are the same)
			else if (height <= Biomes::s_Plains.top && height >= Biomes::s_Plains.bottom)
				mixture.addElement(new PlainsBiome{ m_Seed }, 1.0);

			//desert (no transition since heightmaps are the same)
			else if (height <= Biomes::s_Desert.top && height >= Biomes::s_Desert.bottom)
				mixture.addElement(new DesertBiome{ m_Seed }, 1.0);
		}
	}

	return (const BiomeMixture**) map;
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
void TerrainGenerator::deleteMap(const T** map)
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