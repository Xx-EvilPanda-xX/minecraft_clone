#include <iostream>
#include "Chunk.h"
#include "ChunkManager.h"

Chunk::Chunk(Vector2i loc, Shader& shader) : m_Location{ loc }, m_Mesh{ loc, shader }
{

	for (int i{}; i < g_ChunkCap; ++i)
	{
		m_Sections[i] = nullptr;
	}

	m_CurrentSectionIndex = 0;
	m_Complete = false;
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
	for (int i{}; i < g_ChunkCap; ++i)
	{
		if (m_Sections[i]->isEmpty()) {
			continue;
		}

		for (int x{}; x < 16; ++x)
		{
			for (int y{}; y < 16; ++y)
			{
				for (int z{}; z < 16; ++z)
				{
					if (manager.getWorldBlock(Vector3i{ x, y, z }).getType() == BlockType::Air)
					{
						continue;
					}
					else
					{
						if (manager.getWorldBlock(Vector3i{ x + 1, y, z }).getType() == BlockType::Air)
						{
							m_Mesh.addFace(Vector3i{ x, (i * 16) + y, z }, Face::North);
						}

						if (manager.getWorldBlock(Vector3i{ x, y + 1, z }).getType() == BlockType::Air)
						{
							m_Mesh.addFace(Vector3i{ x, (i * 16) + y, z }, Face::Up);
						}

						if (manager.getWorldBlock(Vector3i{ x, y, z + 1 }).getType() == BlockType::Air)
						{
							m_Mesh.addFace(Vector3i{ x, (i * 16) + y, z }, Face::West);
						}

						if (manager.getWorldBlock(Vector3i{ x - 1, y, z }).getType() == BlockType::Air)
						{
							m_Mesh.addFace(Vector3i{ x, (i * 16) + y, z }, Face::South);
						}

						if (manager.getWorldBlock(Vector3i{ x, y - 1, z }).getType() == BlockType::Air)
						{
							m_Mesh.addFace(Vector3i{ x, (i * 16) + y, z }, Face::Down);
						}

						if (manager.getWorldBlock(Vector3i{ x, y, z - 1 }).getType() == BlockType::Air)
						{
							m_Mesh.addFace(Vector3i{ x, (i * 16) + y, z }, Face::East);
						}
					}
				}
			}
		}
	}
	m_Mesh.toBuffers();
	
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

const Vector2i& Chunk::getLocation() const
{
	return m_Location; 
}

ChunkSection* Chunk::getSection(int index) const
{
	return m_Sections[index];
}