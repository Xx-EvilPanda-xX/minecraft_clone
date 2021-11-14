#include <iostream>
#include "Chunk.h"
#include "ChunkManager.h"

Chunk::Chunk(Vector2i loc, Shader& shader) : m_Location{ loc }, m_Mesh{ Vector2i{ loc.x * 16, loc.y * 16 }, shader }
{

	for (int i{}; i < g_ChunkCap; ++i)
	{
		m_Sections[i] = nullptr;
	}

	m_CurrentSectionIndex = 0;
	m_Complete = false;
	m_IsBuilt = false;
}

Chunk::~Chunk()
{
	for (int i{}; i < g_ChunkCap; ++i)
	{
		delete[] m_Sections[i];
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

void Chunk::buildMesh(ChunkManager& manager)
{
	static int i{};

	if (m_Sections[i]->isEmpty())
	{
		++i;

		if (i == g_ChunkCap)
		{
			i = 0;
			m_Mesh.toBuffers();
			m_IsBuilt = true;
		}

		return;
	}

	for (int x{}; x < 16; ++x)
	{
		for (int y{}; y < 16; ++y)
		{
			for (int z{}; z < 16; ++z)
			{
				int wX{ (m_Location.x * 16) + x };
				int wY{ (i * 16) + y };
				int wZ{ (m_Location.y * 16) + z };
				Block currentBlock{ manager.getWorldBlock(Vector3i{ wX, wY, wZ }) };
				if (currentBlock.getType() == BlockType::Air)
				{
					continue;
				}
				else
				{
					if (manager.getWorldBlock(Vector3i{ wX + 1, wY, wZ }).getType() == BlockType::Air)
					{
						m_Mesh.addFace(Vector3i{ x, wY, z }, currentBlock, Face::North);
					}

					if (manager.getWorldBlock(Vector3i{ wX, wY + 1, wZ }).getType() == BlockType::Air)
					{
						m_Mesh.addFace(Vector3i{ x, wY, z }, currentBlock, Face::Up);
					}

					if (manager.getWorldBlock(Vector3i{ wX, wY, wZ + 1 }).getType() == BlockType::Air)
					{
						m_Mesh.addFace(Vector3i{ x, wY, z }, currentBlock, Face::East);
					}

					if (manager.getWorldBlock(Vector3i{ wX - 1, wY, wZ }).getType() == BlockType::Air)
					{
						m_Mesh.addFace(Vector3i{ x, wY, z }, currentBlock, Face::South);
					}

					if (manager.getWorldBlock(Vector3i{ wX, wY - 1, wZ }).getType() == BlockType::Air)
					{
						m_Mesh.addFace(Vector3i{ x, wY, z }, currentBlock, Face::Down);
					}

					if (manager.getWorldBlock(Vector3i{ wX, wY, wZ - 1 }).getType() == BlockType::Air)
					{
						m_Mesh.addFace(Vector3i{ x, wY, z }, currentBlock, Face::West);
					}
				}
			}
		}
	}
	++i;
	
	if (i == g_ChunkCap)
	{
		i = 0;
		m_Mesh.toBuffers();
		m_IsBuilt = true;
	}
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