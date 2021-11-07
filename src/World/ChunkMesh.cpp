#include <glad/glad.h>
#include "ChunkMesh.h"
#include "Chunk.h"

ChunkMesh::ChunkMesh(Vector2i pos, Shader& shader) : m_Pos{ pos }, m_RenderData{ 0, 0, 0, 0, 0, shader }
{
	hasValidObjects = false;
	/*
	m_Vertices = std::vector<float>{ -0.5, -0.5, 0.0,
									   0.5, 0.5, 0.0,
									   0.5, -0.5, 0.0,
									   -0.5, 0.5, 0.0 };

	m_TexCoords = std::vector<float>{ 0.0, 0.0, 0.0,
								   0.0, 0.0, 0.0,
								   0.0, 0.0, 0.0, 
								   0.0, 0.0, 0.0 };


	m_Indices = std::vector<int>{ 1, 2, 3, 0, 2, 3 };
	*/

	//m_Vertices = std::vector<float>;
	//m_TexCoords = std::vector<float>;
	//m_Indices = std::vector<int>;
}

ChunkMesh::~ChunkMesh()
{
}

void ChunkMesh::addFace(Vector3i loc, Block& block, Face face)
{
	int s = pushNewVertices(loc);
	pushNewTexCoords(block);

	int s0{ s + 0 };
	int s1{ s + 1 };
	int s2{ s + 2 };
	int s3{ s + 3 };
	int s4{ s + 4 };
	int s5{ s + 5 };
	int s6{ s + 6 };
	int s7{ s + 7 };

	switch (face)
	{
	case Face::Up:
		pushInt(s5).pushInt(s6).pushInt(s7).pushInt(s2).pushInt(s6).pushInt(s5);
		break;

	case Face::Down:
		pushInt(s1).pushInt(s3).pushInt(s4).pushInt(s0).pushInt(s3).pushInt(s1);
		break;

	case Face::North:
		pushInt(s4).pushInt(s5).pushInt(s7).pushInt(s1).pushInt(s5).pushInt(s4);
		break;

	case Face::South:
		pushInt(s2).pushInt(s3).pushInt(s6).pushInt(s0).pushInt(s3).pushInt(s2);
		break;

	case Face::East:
		pushInt(s4).pushInt(s6).pushInt(s7).pushInt(s3).pushInt(s6).pushInt(s4);
		break;

	case Face::West:
		pushInt(s1).pushInt(s2).pushInt(s5).pushInt(s0).pushInt(s2).pushInt(s1);
		break;
	}
}

size_t ChunkMesh::pushNewVertices(Vector3i loc)
{
	glm::vec3 floats{ static_cast<float>(loc.x), static_cast<float>(loc.y) , static_cast<float>(loc.z) };
	size_t size{ m_Vertices.size() };

	pushVertexFloat(floats.x).pushVertexFloat(floats.y).pushVertexFloat(floats.z);
	pushVertexFloat(floats.x + 1.0f).pushVertexFloat(floats.y).pushVertexFloat(floats.z);
	pushVertexFloat(floats.x).pushVertexFloat(floats.y + 1.0f).pushVertexFloat(floats.z);
	pushVertexFloat(floats.x).pushVertexFloat(floats.y).pushVertexFloat(floats.z + 1.0f);
	pushVertexFloat(floats.x + 1.0f).pushVertexFloat(floats.y).pushVertexFloat(floats.z + 1.0f);
	pushVertexFloat(floats.x + 1.0f).pushVertexFloat(floats.y + 1.0f).pushVertexFloat(floats.z);
	pushVertexFloat(floats.x).pushVertexFloat(floats.y + 1.0f).pushVertexFloat(floats.z + 1.0f);
	pushVertexFloat(floats.x + 1.0f).pushVertexFloat(floats.y + 1.0f).pushVertexFloat(floats.z + 1.0f);

	return size;
}

size_t ChunkMesh::pushNewTexCoords(Block& block)
{
	size_t size{ m_TexCoords.size() };

	pushVertexFloat(0.0f).pushVertexFloat(0.0f).pushVertexFloat(0.0f);
	pushVertexFloat(0.0f).pushVertexFloat(0.0f).pushVertexFloat(0.0f);
	pushVertexFloat(0.0f).pushVertexFloat(0.0f).pushVertexFloat(0.0f);
	pushVertexFloat(0.0f).pushVertexFloat(0.0f).pushVertexFloat(0.0f);
	pushVertexFloat(0.0f).pushVertexFloat(0.0f).pushVertexFloat(0.0f);
	pushVertexFloat(0.0f).pushVertexFloat(0.0f).pushVertexFloat(0.0f);
	pushVertexFloat(0.0f).pushVertexFloat(0.0f).pushVertexFloat(0.0f);
	pushVertexFloat(0.0f).pushVertexFloat(0.0f).pushVertexFloat(0.0f);

	return size;
}

ChunkMesh& ChunkMesh::pushVertexFloat(float f)
{
	m_Vertices.push_back(f);
	return *this;
}

ChunkMesh& ChunkMesh::pushTexFloat(float f)
{
	m_TexCoords.push_back(f);
	return *this;
}

ChunkMesh& ChunkMesh::pushInt(int i)
{
	m_Indices.push_back(i);
	return *this;
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

	delete[] dataArray;
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
	delete[] dataArray;
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

const Vector2i& ChunkMesh::getPostion() const
{
	return m_Pos;
}