#include <glad/glad.h>
#include "ChunkMesh.h"
#include "Chunk.h"

ChunkMesh::ChunkMesh(Vector2i pos, Shader& shader) : m_Pos{ pos }, m_RenderData{ 0, 0, 0, 0, 0, shader }
{
	hasValidObjects = false;

	m_Vertices = std::vector<float>{ -0.5, -0.5, 5.0,
									   0.5, 0.5, 5.0,
									   0.5, -0.5, 5.0,
									   -0.5, 0.5, 5.0 };

	m_TexCoords = std::vector<float>{ 0.0, 0.0, 0.0,
								   0.0, 0.0, 0.0,
								   0.0, 0.0, 0.0,
								   0.0, 0.0, 0.0 };


	m_Indices = std::vector<int>{ 1, 2, 3, 0, 2, 3 };
}

ChunkMesh::~ChunkMesh()
{
}

void ChunkMesh::addFace(Vector3i loc, Face face)
{
}

void ChunkMesh::enableAttribs() const
{
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
}

void ChunkMesh::disableAttribs() const
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

	float* dataArray{ new float[data.size()]{} };
	for (int i{}; i < data.size(); ++i)
	{
		dataArray[i] = data[i];
	}

	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), dataArray, GL_STATIC_DRAW);
	glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void ChunkMesh::storeIndices(std::vector<int>& data)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RenderData.ebo);

	int* dataArray{ new int[data.size()]{} };
	for (int i{}; i < data.size(); ++i)
	{
		dataArray[i] = data[i];
	}
	
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.size() * sizeof(int), dataArray, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	m_RenderData.indexCount = static_cast<int>(data.size());
}

const std::vector<float>& ChunkMesh::getVertices() const
{
	return m_Vertices;
}

const std::vector<float>& ChunkMesh::getTexCoords() const
{
	return m_TexCoords;
}

const std::vector<int>& ChunkMesh::getIndices() const
{
	return m_Indices;
}

const RenderData& ChunkMesh::getRenderData() const
{
	return m_RenderData;
}

Vector2i ChunkMesh::getPostion() const
{
	return m_Pos;
}