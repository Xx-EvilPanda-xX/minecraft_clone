#include <cmath>
#include <glad/glad.h>
#include "ChunkMesh.h"
#include "Chunk.h"

ChunkMesh::ChunkMesh(Vector2i pos, Shader& shader) : m_Pos{ pos }, m_RenderData{ 0, 0, 0, 0, 0, 0, shader }
{
	hasValidObjects = false;
}

Texture ChunkMesh::s_TexAltas{ Texture{} };
BlockType ChunkMesh::s_AtlasIndices[]{ BlockType::Grass, BlockType::Stone };

void ChunkMesh::createTextureAtlas(const char* path)
{
	s_TexAltas = Texture{ path };
}

void ChunkMesh::addFace(Vector3i loc, Block block, Face face)
{
	switch (face)
	{
	case Face::Up:
		pushUp(loc);
		break;

	case Face::Down:
		pushDown(loc);
		break;

	case Face::North:
		pushNorth(loc);
		break;

	case Face::South:
		pushSouth(loc);
		break;

	case Face::East:
		pushEast(loc);
		break;

	case Face::West:
		pushWest(loc);
		break;
	}

	float* tex{ calcTexCoords(block.getType(), face) };
	for (int i{}; i < 8; ++i)
	{
		m_TexCoords.push_back(tex[i]);
	}
}

float* ChunkMesh::calcTexCoords(BlockType block, Face face)
{
	float coords[8]{};

	float index{};
	for (int i{}; i < BlockType::NumBlocks; ++i)
	{
		if (s_AtlasIndices[i] == block)
			index = i;
	}

	float fullIndex{ index * 0.5f };

	//extract integer part and decimal part of float
	float x{ static_cast<float>(static_cast<int>(fullIndex)) };
	float y{ fullIndex - x };
	
	glm::vec2 startCoord{ x, y };

	switch (face)
	{
	case Face::Up:
		return getTexCoordsFromStartPos(startCoord);
	case Face::North:
		return getTexCoordsFromStartPos(startCoord + 0.1f);
	case Face::East:
		return getTexCoordsFromStartPos(startCoord + 0.2f);
	case Face::South:
		return getTexCoordsFromStartPos(startCoord + 0.3f);
	case Face::West:
		return getTexCoordsFromStartPos(startCoord + 0.4f);
	case Face::Down:
		return getTexCoordsFromStartPos(startCoord + 0.5f);
	}


}

float* ChunkMesh::getTexCoordsFromStartPos(glm::vec2 startPos)
{
	float coords[8]{};
	
	coords[0] = startPos.x;
	coords[1] = startPos.y;

	coords[2] = startPos.x + 0.1f;
	coords[3] = startPos.y;

	coords[4] = startPos.x;
	coords[5] = startPos.y + 0.1f;

	coords[6] = startPos.x + 0.1f;
	coords[7] = startPos.y + 0.1f;

	return coords;
}

void ChunkMesh::pushUp(Vector3i loc)
{
	int size{ static_cast<int>(m_Vertices.size()) / 3 };
	glm::vec3 floats{ static_cast<float>(loc.x), static_cast<float>(loc.y) , static_cast<float>(loc.z) };

	pushVertexFloat(floats.x).pushVertexFloat(floats.y + 1.0f).pushVertexFloat(floats.z); //2
	pushVertexFloat(floats.x + 1.0f).pushVertexFloat(floats.y + 1.0f).pushVertexFloat(floats.z); //5
	pushVertexFloat(floats.x).pushVertexFloat(floats.y + 1.0f).pushVertexFloat(floats.z + 1.0f); //6
	pushVertexFloat(floats.x + 1.0f).pushVertexFloat(floats.y + 1.0f).pushVertexFloat(floats.z + 1.0f); //7

	//pushTexFloat(0.0f).pushTexFloat(0.0f).pushTexFloat(0.0f).pushTexFloat(0.0f).pushTexFloat(0.0f).pushTexFloat(0.0f).pushTexFloat(0.0f).pushTexFloat(0.0f);
	pushLighting(1.0f).pushLighting(1.0f).pushLighting(1.0f).pushLighting(1.0f);

	pushNewIndices(size);
}

void ChunkMesh::pushDown(Vector3i loc)
{
	int size{ static_cast<int>(m_Vertices.size()) / 3 };
	glm::vec3 floats{ static_cast<float>(loc.x), static_cast<float>(loc.y) , static_cast<float>(loc.z) };

	pushVertexFloat(floats.x).pushVertexFloat(floats.y).pushVertexFloat(floats.z); //0
	pushVertexFloat(floats.x + 1.0f).pushVertexFloat(floats.y).pushVertexFloat(floats.z); //1
	pushVertexFloat(floats.x).pushVertexFloat(floats.y).pushVertexFloat(floats.z + 1.0f); //3
	pushVertexFloat(floats.x + 1.0f).pushVertexFloat(floats.y).pushVertexFloat(floats.z + 1.0f); //4

	//pushTexFloat(0.0f).pushTexFloat(0.0f).pushTexFloat(0.0f).pushTexFloat(0.0f).pushTexFloat(0.0f).pushTexFloat(0.0f).pushTexFloat(0.0f).pushTexFloat(0.0f);
	pushLighting(0.6f).pushLighting(0.6f).pushLighting(0.6f).pushLighting(0.6f);

	pushNewIndices(size);
}

void ChunkMesh::pushNorth(Vector3i loc)
{
	int size{ static_cast<int>(m_Vertices.size()) / 3 };
	glm::vec3 floats{ static_cast<float>(loc.x), static_cast<float>(loc.y) , static_cast<float>(loc.z) };

	pushVertexFloat(floats.x + 1.0f).pushVertexFloat(floats.y).pushVertexFloat(floats.z); //1
	pushVertexFloat(floats.x + 1.0f).pushVertexFloat(floats.y).pushVertexFloat(floats.z + 1.0f); //4
	pushVertexFloat(floats.x + 1.0f).pushVertexFloat(floats.y + 1.0f).pushVertexFloat(floats.z); //5
	pushVertexFloat(floats.x + 1.0f).pushVertexFloat(floats.y + 1.0f).pushVertexFloat(floats.z + 1.0f); //7

	//pushTexFloat(0.0f).pushTexFloat(0.0f).pushTexFloat(0.0f).pushTexFloat(0.0f).pushTexFloat(0.0f).pushTexFloat(0.0f).pushTexFloat(0.0f).pushTexFloat(0.0f);
	pushLighting(0.8f).pushLighting(0.8f).pushLighting(0.8f).pushLighting(0.8f);

	pushNewIndices(size);
}

void ChunkMesh::pushSouth(Vector3i loc)
{
	int size{ static_cast<int>(m_Vertices.size()) / 3 };
	glm::vec3 floats{ static_cast<float>(loc.x), static_cast<float>(loc.y) , static_cast<float>(loc.z) };

	pushVertexFloat(floats.x).pushVertexFloat(floats.y).pushVertexFloat(floats.z); //0
	pushVertexFloat(floats.x).pushVertexFloat(floats.y + 1.0f).pushVertexFloat(floats.z); //2
	pushVertexFloat(floats.x).pushVertexFloat(floats.y).pushVertexFloat(floats.z + 1.0f); //3
	pushVertexFloat(floats.x).pushVertexFloat(floats.y + 1.0f).pushVertexFloat(floats.z + 1.0f); //6

	//pushTexFloat(0.0f).pushTexFloat(0.0f).pushTexFloat(0.0f).pushTexFloat(0.0f).pushTexFloat(0.0f).pushTexFloat(0.0f).pushTexFloat(0.0f).pushTexFloat(0.0f);
	pushLighting(0.8f).pushLighting(0.8f).pushLighting(0.8f).pushLighting(0.8f);

	pushNewIndices(size);
}

void ChunkMesh::pushEast(Vector3i loc)
{
	int size{ static_cast<int>(m_Vertices.size()) / 3 };
	glm::vec3 floats{ static_cast<float>(loc.x), static_cast<float>(loc.y) , static_cast<float>(loc.z) };

	pushVertexFloat(floats.x).pushVertexFloat(floats.y).pushVertexFloat(floats.z + 1.0f); //3
	pushVertexFloat(floats.x + 1.0f).pushVertexFloat(floats.y).pushVertexFloat(floats.z + 1.0f); //4
	pushVertexFloat(floats.x).pushVertexFloat(floats.y + 1.0f).pushVertexFloat(floats.z + 1.0f); //6
	pushVertexFloat(floats.x + 1.0f).pushVertexFloat(floats.y + 1.0f).pushVertexFloat(floats.z + 1.0f); //7

	//pushTexFloat(0.0f).pushTexFloat(0.0f).pushTexFloat(0.0f).pushTexFloat(0.0f).pushTexFloat(0.0f).pushTexFloat(0.0f).pushTexFloat(0.0f).pushTexFloat(0.0f);
	pushLighting(0.8f).pushLighting(0.8f).pushLighting(0.8f).pushLighting(0.8f);

	pushNewIndices(size);
}

void ChunkMesh::pushWest(Vector3i loc)
{
	glm::vec3 floats{ static_cast<float>(loc.x), static_cast<float>(loc.y) , static_cast<float>(loc.z) };
	int size{ static_cast<int>(m_Vertices.size()) / 3 };

	pushVertexFloat(floats.x).pushVertexFloat(floats.y).pushVertexFloat(floats.z); //0
	pushVertexFloat(floats.x + 1.0f).pushVertexFloat(floats.y).pushVertexFloat(floats.z); //1
	pushVertexFloat(floats.x).pushVertexFloat(floats.y + 1.0f).pushVertexFloat(floats.z); //2
	pushVertexFloat(floats.x + 1.0f).pushVertexFloat(floats.y + 1.0f).pushVertexFloat(floats.z); //5

	//pushTexFloat(0.0f).pushTexFloat(0.0f).pushTexFloat(0.0f).pushTexFloat(0.0f).pushTexFloat(0.0f).pushTexFloat(0.0f).pushTexFloat(0.0f).pushTexFloat(0.0f);
	pushLighting(0.8f).pushLighting(0.8f).pushLighting(0.8f).pushLighting(0.8f);
	
	pushNewIndices(size);
}

void ChunkMesh::pushNewIndices(int size)
{
	int s0{ size + 0 };
	int s1{ size + 1 };
	int s2{ size + 2 };
	int s3{ size + 3 };

	pushIndex(s1).pushIndex(s2).pushIndex(s3).pushIndex(s0).pushIndex(s2).pushIndex(s1);
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

ChunkMesh& ChunkMesh::pushLighting(float f)
{
	m_Lighting.push_back(f);
	return *this;
}

ChunkMesh& ChunkMesh::pushIndex(int i)
{
	m_Indices.push_back(i);
	return *this;
}

void ChunkMesh::enableAttribs() const
{
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
}

void ChunkMesh::disableAttribs() const
{
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}

void ChunkMesh::toBuffers()
{
	if (hasValidObjects)
	{
		glDeleteVertexArrays(1, &m_RenderData.vao);

		glDeleteBuffers(1, &m_RenderData.vbo);
		glDeleteBuffers(1, &m_RenderData.tbo);
		glDeleteBuffers(1, &m_RenderData.ebo);
		glDeleteBuffers(1, &m_RenderData.lbo);
	}

	glGenVertexArrays(1, &m_RenderData.vao);

	glGenBuffers(1, &m_RenderData.vbo);
	glGenBuffers(1, &m_RenderData.tbo);
	glGenBuffers(1, &m_RenderData.ebo);
	glGenBuffers(1, &m_RenderData.lbo);

	storeFloatBuffer(0, 3, m_RenderData.vbo, m_Vertices);
	storeFloatBuffer(1, 2, m_RenderData.tbo, m_TexCoords);
	storeFloatBuffer(2, 1, m_RenderData.lbo, m_Lighting);

	storeIndices(m_Indices);

	m_RenderData.texture = s_TexAltas.getId();

	hasValidObjects = true;
}

void ChunkMesh::storeFloatBuffer(int index, int size, int buffer, const std::vector<float>& data)
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