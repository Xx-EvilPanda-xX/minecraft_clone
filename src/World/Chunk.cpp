#include <iostream>
#include "Chunk.h"

Chunk::Chunk()
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
	delete[] m_Sections;
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

void Chunk::buildMesh()
{
	for (int i{}; i < g_ChunkCap; ++i)
	{
		if (m_Sections[i]->isFull()) {
			//continue;
		}

		for (int x{}; x < 16; ++x)
		{
			for (int y{}; y < 16; ++y)
			{
				for (int z{}; z < 16; ++z)
				{
					if (m_Sections[i]->getBlock(Vector3i{ x, y, z }).getType() == BlockType::Air)
					{
						continue;
					}
					else
					{
						if (m_Sections[i]->getBlock(Vector3i{ x + 1, y, z }).getType() != BlockType::Air)
						{
							m_Mesh.addFace(Vector3i{ x, (i * 16) + y, z }, Face::North);
						}

						if (m_Sections[i]->getBlock(Vector3i{ x, y + 1, z }).getType() != BlockType::Air)
						{
							m_Mesh.addFace(Vector3i{ x, (i * 16) + y, z }, Face::Up);
						}

						if (m_Sections[i]->getBlock(Vector3i{ x, y, z + 1 }).getType() != BlockType::Air)
						{
							m_Mesh.addFace(Vector3i{ x, (i * 16) + y, z }, Face::West);
						}

						if (m_Sections[i]->getBlock(Vector3i{ x - 1, y, z }).getType() != BlockType::Air)
						{
							m_Mesh.addFace(Vector3i{ x, (i * 16) + y, z }, Face::South);
						}

						if (m_Sections[i]->getBlock(Vector3i{ x, y - 1, z }).getType() != BlockType::Air)
						{
							m_Mesh.addFace(Vector3i{ x, (i * 16) + y, z }, Face::Down);
						}

						if (m_Sections[i]->getBlock(Vector3i{ x, y, z - 1 }).getType() != BlockType::Air)
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