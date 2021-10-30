#include <glad/glad.h>
#include "ChunkMesh.h"
#include "Chunk.h"

ChunkMesh::ChunkMesh()
{
	hasValidObjects = false;
	m_TempHasAddedFace = false;

	m_RenderData = RenderData{};
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

		m_TexCoords = std::vector<float>{ 0.0, 0.0, 0.0,
									   0.0, 0.0, 0.0,
									   0.0, 0.0, 0.0,
									   0.0, 0.0, 0.0 };
		

		m_Indices = std::vector<int>{ 1, 2, 3, 4, 2, 3 };
		m_TempHasAddedFace = true;
	}
}

void ChunkMesh::enableAttribs()
{
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
}

void ChunkMesh::disableAttribs()
{
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

void ChunkMesh::toBuffers()
{
	if (hasValidObjects)
	{
		glDeleteVertexArrays(1, &m_RenderData.vao);

		glDeleteBuffers(1, &m_RenderData.vbo);
		glDeleteBuffers(1, &m_RenderData.tbo);
		glDeleteBuffers(1, &m_RenderData.ebo);
	}

	glGenVertexArrays(1, &m_RenderData.vao);

	glGenBuffers(1, &m_RenderData.vbo);
	glGenBuffers(1, &m_RenderData.tbo);
	glGenBuffers(1, &m_RenderData.ebo);

	storeBuffer(0, 3, m_RenderData.vbo, m_Vertices);
	storeBuffer(1, 2, m_RenderData.tbo, m_TexCoords);

	storeIndices(m_Indices);

	hasValidObjects = true;
}

void ChunkMesh::storeBuffer(int index, int size, int buffer, std::vector<float>& data)
{
	glBindVertexArray(m_RenderData.vao);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
	glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void ChunkMesh::storeIndices(std::vector<int>& data)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RenderData.ebo);
	
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.size() * sizeof(int), &data[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	m_RenderData.indexCount = data.size();
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

const RenderData& ChunkMesh::getRenderData()
{
	return m_RenderData;
}