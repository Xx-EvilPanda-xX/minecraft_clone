#include <iostream>
#include "Chunk.h"
#include "ChunkManager.h"

Chunk::Chunk(Vector2i loc, Shader& shader) 
	: m_Location{ loc },
	m_Mesh{ Vector2i{ loc.x * 16, loc.y * 16 }, shader }
{

	for (int i{}; i < g_ChunkCap; ++i)
	{
		m_Sections[i] = nullptr;
	}

	m_CurrentSectionIndex = 0;
	m_Complete = false;
	m_IsBuilt = false;

	for (int i{}; i < g_ChunkCap; ++i)
	{
		m_RemainingSections.push_back(i);
	}
}

Chunk::~Chunk()
{
	for (int i{}; i < m_CurrentSectionIndex; ++i)
	{
		delete m_Sections[i];
		m_Sections[i] = nullptr;
	}

	if (m_IsBuilt)
		m_Mesh.clear();
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

void Chunk::buildMesh(ChunkManager& manager, int section)
{
	if (section < 0 || section > 16)
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

	if (m_Sections[section]->isEmpty())
	{
		if (m_RemainingSections.size() == 0)
		{
			m_Mesh.toBuffers();
			m_IsBuilt = true;
		}

		return;
	}

	ChunkSection* chunkSection{ m_Sections[section] };

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
				if (x == 15 || x == 0 || y == 15 || y == 0 || z == 15 || z == 0)
					currentBlock = manager.getWorldBlock(Vector3i{ wX, wY, wZ });
				else
					currentBlock = chunkSection->getBlock(Vector3i{ x, y, z });

				if (currentBlock.getType() == BlockType::Air)
				{
					continue;
				}
				else
				{
					if (x == 15 || x == 0 || y == 15 || y == 0 || z == 15 || z == 0)
					{
						Block PosX{ manager.getWorldBlock(Vector3i{ wX + 1, wY, wZ }) };
						if (PosX.isTransparent())
						{
							m_Mesh.addFace(Vector3i{ x, wY, z }, currentBlock, Face::North);
						}

						Block PosY{ manager.getWorldBlock(Vector3i{ wX, wY + 1, wZ }) };
						if (PosY.isTransparent())
						{
							m_Mesh.addFace(Vector3i{ x, wY, z }, currentBlock, Face::Up);
						}

						Block PosZ{ manager.getWorldBlock(Vector3i{ wX, wY , wZ + 1 }) };
						if (PosZ.isTransparent())
						{
							m_Mesh.addFace(Vector3i{ x, wY, z }, currentBlock, Face::East);
						}

						Block NegX{ manager.getWorldBlock(Vector3i{ wX - 1, wY, wZ }) };
						if (NegX.isTransparent())
						{
							m_Mesh.addFace(Vector3i{ x, wY, z }, currentBlock, Face::South);
						}

						Block NegY{ manager.getWorldBlock(Vector3i{ wX, wY - 1, wZ }) };
						if (NegY.isTransparent())
						{
							m_Mesh.addFace(Vector3i{ x, wY, z }, currentBlock, Face::Down);
						}

						Block NegZ{ manager.getWorldBlock(Vector3i{ wX, wY, wZ - 1 }) };
						if (NegZ.isTransparent())
						{
							m_Mesh.addFace(Vector3i{ x, wY, z }, currentBlock, Face::West);
						}
					}
					else
					{
						Block PosX{ chunkSection->getBlock(Vector3i{ x + 1, y, z }) };
						if (PosX.isTransparent())
						{
							m_Mesh.addFace(Vector3i{ x, wY, z }, currentBlock, Face::North);
						}

						Block PosY{ chunkSection->getBlock(Vector3i{ x, y + 1, z }) };
						if (PosY.isTransparent())
						{
							m_Mesh.addFace(Vector3i{ x, wY, z }, currentBlock, Face::Up);
						}

						Block PosZ{ chunkSection->getBlock(Vector3i{ x, y, z + 1 }) };
						if (PosZ.isTransparent())
						{
							m_Mesh.addFace(Vector3i{ x, wY, z }, currentBlock, Face::East);
						}

						Block NegX{ chunkSection->getBlock(Vector3i{ x - 1, y, z }) };
						if (NegX.isTransparent())
						{
							m_Mesh.addFace(Vector3i{ x, wY, z }, currentBlock, Face::South);
						}

						Block NegY{ chunkSection->getBlock(Vector3i{ x, y - 1, z }) };
						if (NegY.isTransparent())
						{
							m_Mesh.addFace(Vector3i{ x, wY, z }, currentBlock, Face::Down);
						}

						Block NegZ{ chunkSection->getBlock(Vector3i{ x, y, z - 1 }) };
						if (NegZ.isTransparent())
						{
							m_Mesh.addFace(Vector3i{ x, wY, z }, currentBlock, Face::West);
						}
					}
				}
			}
		}
	}
	
	if (m_RemainingSections.size() == 0)
	{
		m_Mesh.toBuffers();
		m_IsBuilt = true;
	}
}

void Chunk::clearMesh()
{
	m_Mesh.clear();
	m_IsBuilt = false;
}

int Chunk::getCurrentSectionIndex() const
{
	return m_CurrentSectionIndex;
}

const ChunkMesh& Chunk::getMesh() const
{
	return m_Mesh;
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