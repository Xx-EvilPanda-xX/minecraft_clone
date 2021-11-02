#include <iostream>
#include "Chunk.h"
#include "ChunkManager.h"

Chunk::Chunk(glm::vec2  loc) : m_Location{ loc }
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
					if (manager.getWorldBlock(glm::vec3{ x, y, z }).getType() == BlockType::Air)
					{
						continue;
					}
					else
					{
						if (manager.getWorldBlock(glm::vec3{ x + 1, y, z }).getType() == BlockType::Air)
						{
							m_Mesh.addFace(glm::vec3{ x, (i * 16) + y, z }, Face::North);
						}

						if (manager.getWorldBlock(glm::vec3{ x, y + 1, z }).getType() == BlockType::Air)
						{
							m_Mesh.addFace(glm::vec3{ x, (i * 16) + y, z }, Face::Up);
						}

						if (manager.getWorldBlock(glm::vec3{ x, y, z + 1 }).getType() == BlockType::Air)
						{
							m_Mesh.addFace(glm::vec3{ x, (i * 16) + y, z }, Face::West);
						}

						if (manager.getWorldBlock(glm::vec3{ x - 1, y, z }).getType() == BlockType::Air)
						{
							m_Mesh.addFace(glm::vec3{ x, (i * 16) + y, z }, Face::South);
						}

						if (manager.getWorldBlock(glm::vec3{ x, y - 1, z }).getType() == BlockType::Air)
						{
							m_Mesh.addFace(glm::vec3{ x, (i * 16) + y, z }, Face::Down);
						}

						if (manager.getWorldBlock(glm::vec3{ x, y, z - 1 }).getType() == BlockType::Air)
						{
							m_Mesh.addFace(glm::vec3{ x, (i * 16) + y, z }, Face::East);
						}
					}
				}
			}
		}
	}
	m_Mesh.toBuffers();
	
}

int Chunk::getCurrentSectionIndex()
{
	return m_CurrentSectionIndex;
}

ChunkMesh& Chunk::getMesh()
{
	return m_Mesh;
}

bool Chunk::isComplete()
{
	return m_Complete;
}

glm::vec2  Chunk::getLocation()
{
	return m_Location;
}

ChunkSection* Chunk::getSection(int index)
{
	return m_Sections[index];
}