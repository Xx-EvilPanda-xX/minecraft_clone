#include <iostream>
#include "Chunk.h"
#include "ChunkManager.h"
#include "Block.h"
#include "../Constants.h"

Chunk::Chunk(Vector2i loc, Shader& shader) 
	: m_Location{ loc },
	m_SolidMesh{ Vector2i{ loc.x * 16, loc.y * 16 }, shader },
	m_TranslucentMesh{ Vector2i{ loc.x * 16, loc.y * 16 }, shader }
{
	for (int i{}; i < g_ChunkCap; ++i)
	{
		m_Sections[i] = nullptr;
	}

	m_CurrentSectionIndex = 0;
	m_Complete = false;
	m_IsBuilt = false;
	m_Building = false;

	resetRemaining();
}

Chunk::~Chunk()
{
	for (int i{}; i < m_CurrentSectionIndex; ++i)
	{
		delete m_Sections[i];
		m_Sections[i] = nullptr;
	}

	if (m_IsBuilt)
	{
		m_SolidMesh.clear();
		m_TranslucentMesh.clear();
	}
}

void Chunk::addSection(ChunkSection* section)
{
	if (m_Complete)
	{
		std::cout << "Chunk is already complete!\n";
		return;
	}

	m_Sections[m_CurrentSectionIndex] = section;
	++m_CurrentSectionIndex;

	if (m_CurrentSectionIndex == g_ChunkCap)
	{
		m_Complete = true;
	}
}

void Chunk::buildMesh(ChunkManager& manager, int section, Chunk* adjacentChunks[4])
{	
	if (section < 0 || section > 15)
	{
		std::cout << "Invalid section index!\n";
		return;
	}

	for (int i{}; i < m_RemainingSections.size(); ++i)
	{
		if (m_RemainingSections[i] == section)
		{
			m_RemainingSections.erase(m_RemainingSections.begin() + i);
			break;
		}
	}

	bool adjacentSectionsFull{ true };
	for (int i{}; i < 4; ++i)
	{
		if (!adjacentChunks[i]->m_Sections[section]->isFull())
		{
			adjacentSectionsFull = false;
			break;
		}
	}

	if (m_Sections[section]->isEmpty())
	{
		finishBuilding();
		return;
	}

	if (m_Sections[section]->isFull() && adjacentSectionsFull && !(section == 0 || section == 15))
	{
		if (m_Sections[section + 1]->isFull() && m_Sections[section - 1]->isFull())
		{
			finishBuilding();
			return;
		}
	}

	ChunkSection* chunkSection{ m_Sections[section] };
	ChunkSection* sectionPosX{ manager.getChunk({ m_Location.x + 1, m_Location.y })->m_Sections[section] };
	ChunkSection* sectionPosY{ section != 15 ? m_Sections[section + 1] : nullptr };
	ChunkSection* sectionPosZ{ manager.getChunk({ m_Location.x, m_Location.y + 1 })->m_Sections[section] };
	ChunkSection* sectionNegX{ manager.getChunk({ m_Location.x - 1, m_Location.y })->m_Sections[section] };
	ChunkSection* sectionNegY{ section != 0 ? m_Sections[section - 1] : nullptr };
	ChunkSection* sectionNegZ{ manager.getChunk({ m_Location.x, m_Location.y - 1 })->m_Sections[section] };
	m_Building = true;

	for (int x{}; x < 16; ++x)
	{
		for (int y{}; y < 16; ++y)
		{
			for (int z{}; z < 16; ++z)
			{
				int wX{ (m_Location.x * 16) + x };
				int wY{ (section * 16) + y };
				int wZ{ (m_Location.y * 16) + z };

				Block currentBlock;
				Block PosX;
				Block PosY;
				Block PosZ;
				Block NegX;
				Block NegY;
				Block NegZ;

				currentBlock = chunkSection->getBlock(Vector3i{ x, y, z });

				if (currentBlock.getType() == BlockType::Air)
					continue;

				if (x == 15 || x == 0 || y == 15 || y == 0 || z == 15 || z == 0)
				{
					PosX = (x == 15 ? sectionPosX : chunkSection)->getBlock(Vector3i(x == 15 ? 0 : x + 1, y, z));
					NegX = (x == 0 ? sectionNegX : chunkSection)->getBlock(Vector3i(x == 0 ? 15 : x - 1, y, z));

					if (sectionPosY)
						PosY = (y == 15 ? sectionPosY : chunkSection)->getBlock(Vector3i(x, y == 15 ? 0 : y + 1, z));
					else
						PosY = manager.getWorldBlock(Vector3i{ wX, wY + 1, wZ });

					if (sectionNegY)
						NegY = (y == 0 ? sectionNegY : chunkSection)->getBlock(Vector3i(x, y == 0 ? 15 : y - 1, z));
					else
						NegY = manager.getWorldBlock(Vector3i{ wX, wY - 1, wZ });
					
					PosZ = (z == 15 ? sectionPosZ : chunkSection)->getBlock(Vector3i(x, y, z == 15 ? 0 : z + 1));
					NegZ = (z == 0 ? sectionNegZ : chunkSection)->getBlock(Vector3i(x, y, z == 0 ? 15 : z - 1));
				}
				else
				{
					PosX = chunkSection->getBlock(Vector3i{ x + 1, y, z });
					PosY = chunkSection->getBlock(Vector3i{ x, y + 1, z });
					PosZ = chunkSection->getBlock(Vector3i{ x, y, z + 1 });
					NegX = chunkSection->getBlock(Vector3i{ x - 1, y, z });
					NegY = chunkSection->getBlock(Vector3i{ x, y - 1, z });
					NegZ = chunkSection->getBlock(Vector3i{ x, y, z - 1 });
				}

				tryAddFace(PosX, currentBlock, Face::North, Vector3i{ x, wY, z });
				tryAddFace(PosY, currentBlock, Face::Up, Vector3i{ x, wY, z });
				tryAddFace(PosZ, currentBlock, Face::East, Vector3i{ x, wY, z });
				tryAddFace(NegX, currentBlock, Face::South, Vector3i{ x, wY, z });
				tryAddFace(NegY, currentBlock, Face::Down, Vector3i{ x, wY, z });
				tryAddFace(NegZ, currentBlock, Face::West, Vector3i{ x, wY, z });
			}
		}
	}
	
	finishBuilding();
}

void Chunk::tryAddFace(Block testBlock, Block currentBlock, Face face, Vector3i pos)
{
	if (testBlock.getType() == BlockType::Air ||
		(!currentBlock.isTransparent() && testBlock.isTransparent()) ||
		(currentBlock.isTransparent() && testBlock.isTransparent() && currentBlock.getType() != testBlock.getType()))
	{
		if (currentBlock.getType() == BlockType::Water && testBlock.getType() == BlockType::Air && constants::useTranslucentWater)
			m_TranslucentMesh.addFace(pos, currentBlock, face);
		else
			m_SolidMesh.addFace(pos, currentBlock, face);
	}
}

void Chunk::finishBuilding()
{
	if (m_RemainingSections.size() == 0)
	{
		m_SolidMesh.toBuffers();
		m_TranslucentMesh.toBuffers();
		m_IsBuilt = true;
		m_Building = false;
		resetRemaining();
	}
}

void Chunk::clearMesh()
{
	m_SolidMesh.clear();
	m_TranslucentMesh.clear();
	m_IsBuilt = false;
	m_Building = false;
	resetRemaining();
}

void Chunk::resetRemaining()
{
	m_RemainingSections.clear();
	for (int i{}; i < g_ChunkCap; ++i)
	{
		m_RemainingSections.push_back(i);
	}
}

int Chunk::getCurrentSectionIndex() const
{
	return m_CurrentSectionIndex;
}

const ChunkMesh& Chunk::getSolidMesh() const
{
	return m_SolidMesh;
}

const ChunkMesh& Chunk::getTranslucentMesh() const
{
	return m_TranslucentMesh;
}

bool Chunk::isComplete() const
{
	return m_Complete;
}

const Vector2i Chunk::getLocation() const
{
	return m_Location;
}

ChunkSection* Chunk::getSection(int index) const
{
	return m_Sections[index];
}

bool Chunk::isBuilt() const
{
	return m_IsBuilt;
}