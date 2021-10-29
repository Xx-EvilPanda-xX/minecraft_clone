#include <glad/glad.h>
#include "ChunkMesh.h"
#include "Chunk.h"

ChunkMesh::ChunkMesh()
{
	hasValidObjects = false;
	m_TempHasAddedFace = false;
}

ChunkMesh::~ChunkMesh()
{
}

void ChunkMesh::addFace(Vector3i loc, Face face)
{
	if (!m_TempHasAddedFace)
	{
		m_Vertices = std::vector<float>{ -0.5, -0.5, 0.0,
									   0.5, 0.5, 0.0,
									   0.5, -0.5, 0.0,
									   -0.5, 0.5, 0.0 };

		m_Indices = std::vector<int>{ 1, 2, 3, 4, 2, 3 };
		m_TempHasAddedFace = true;
	}
}

void ChunkMesh::toBuffers()
{
	if (hasValidObjects)
	{
		glDeleteVertexArrays(1, &m_Objects.vao);

		glDeleteBuffers(1, &m_Objects.vbo);
		glDeleteBuffers(1, &m_Objects.tbo);
		glDeleteBuffers(1, &m_Objects.ebo);
	}

	glGenVertexArrays(1, &m_Objects.vao);

	glGenBuffers(1, &m_Objects.vbo);
	glGenBuffers(1, &m_Objects.vbo);
	glGenBuffers(1, &m_Objects.vbo);

	storeBuffer(0, 3, m_Objects.vbo, m_Vertices);
	storeBuffer(1, 2, m_Objects.tbo, m_TexCoords);

	storeIndices(m_Indices);

	hasValidObjects = true;
}

void ChunkMesh::storeBuffer(int index, int size, int buffer, std::vector<float>& data)
{
	glBindVertexArray(m_Objects.vao);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	glBufferData(GL_ARRAY_BUFFER, sizeof(data), &data[0], GL_STATIC_DRAW);
	glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void ChunkMesh::storeIndices(std::vector<int>& data)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Objects.ebo);
	
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(data), &data[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

const std::vector<float>& ChunkMesh::getVertices()
{
	return m_Vertices;
}

const std::vector<float>& ChunkMesh::getTexCoords()
{
	return m_TexCoords;
}

const std::vector<int>& ChunkMesh::getIndices()
{
	return m_Indices;
}

const RenderData& ChunkMesh::getObjects()
{
	return m_Objects;
}