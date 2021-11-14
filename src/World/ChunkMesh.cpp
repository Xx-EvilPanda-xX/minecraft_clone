#include <cmath>
#include <glad/glad.h>
#include "ChunkMesh.h"
#include "Chunk.h"

ChunkMesh::ChunkMesh(Vector2i pos, Shader& shader) : m_Pos{ pos }, m_RenderData{ 0, 0, 0, 0, 0, 0, shader }
{
	hasValidObjects = false;
}

Texture ChunkMesh::s_TexAltas{ Texture{} };
BlockType ChunkMesh::s_AtlasIndices[]{ BlockType::Grass, BlockType::Stone, BlockType::Dirt };

void ChunkMesh::createTextureAtlas(const char* path)
{
	s_TexAltas = Texture{ path };
}

void ChunkMesh::addFace(Vector3i loc, Block block, Face face)
{
	currentFace = face;

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

	delete[] tex;
}

float* ChunkMesh::calcTexCoords(BlockType block, Face face)
{
	int e{};		

	float index{};
	for (int i{}; i < BlockType::NumBlocks; ++i)
	{
		if (s_AtlasIndices[i] == block)
		{
			index = i;
			break;
		}
	}

	float fullIndex{ index * 0.6f };

	//extract integer part and decimal part of float
	float y{ static_cast<float>(static_cast<int>(fullIndex)) };
	float x{ fullIndex - y };
	y /= 10;
	
	glm::vec2 startCoord{ x, y };

	switch (face)
	{
		case Face::Up:
		{
			glm::vec2 faceStartCoords{ startCoord };
			checkTexCoordBounds(faceStartCoords);
			return getTexCoordsFromStartPos(faceStartCoords);
		}	
		case Face::North:
		{
			glm::vec2 faceStartCoords{ startCoord + glm::vec2{ 0.1f, 0.0f } };
			checkTexCoordBounds(faceStartCoords);
			return getTexCoordsFromStartPos(faceStartCoords);
		}
		case Face::East:
		{
			glm::vec2 faceStartCoords{ startCoord + glm::vec2{ 0.2f, 0.0f } };
			checkTexCoordBounds(faceStartCoords);
			return getTexCoordsFromStartPos(faceStartCoords);
		}
		case Face::South:
		{
			glm::vec2 faceStartCoords{ startCoord + glm::vec2{ 0.3f, 0.0f } };
			checkTexCoordBounds(faceStartCoords);
			return getTexCoordsFromStartPos(faceStartCoords);
		}
		case Face::West:
		{
			glm::vec2 faceStartCoords{ startCoord + glm::vec2{ 0.4f, 0.0f } };
			checkTexCoordBounds(faceStartCoords);
			return getTexCoordsFromStartPos(faceStartCoords);
		}
		case Face::Down:
		{
			glm::vec2 faceStartCoords{ startCoord + glm::vec2{ 0.5f, 0.0f } };
			checkTexCoordBounds(faceStartCoords);
			return getTexCoordsFromStartPos(faceStartCoords);
		}
	}
}

void ChunkMesh::checkTexCoordBounds(glm::vec2& faceStartCoords)
{
	if (faceStartCoords.x >= 1.0f)
	{
		faceStartCoords.y += 0.1f;
		faceStartCoords.x = faceStartCoords.x - 1.0f;
	}
}

float* ChunkMesh::getTexCoordsFromStartPos(glm::vec2 startPos)
{
	float* coords{ new float[8]{} };
	
	coords[0] = startPos.x + 0.1f;
	coords[1] = startPos.y + 0.1f;

	coords[2] = startPos.x;
	coords[3] = startPos.y + 0.1f;

	coords[4] = startPos.x + 0.1f;
	coords[5] = startPos.y;

	coords[6] = startPos.x;
	coords[7] = startPos.y;

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
	pushLighting(1.0f).pushLighting(1.0f).pushLighting(1.0f).pushLighting(1.0f);

	pushNewIndices(size);
}

void ChunkMesh::pushDown(Vector3i loc)
{
	int size{ static_cast<int>(m_Vertices.size()) / 3 };
	glm::vec3 floats{ static_cast<float>(loc.x), static_cast<float>(loc.y) , static_cast<float>(loc.z) };
	pushVertexFloat(floats.x + 1.0f).pushVertexFloat(floats.y).pushVertexFloat(floats.z); //1
	pushVertexFloat(floats.x).pushVertexFloat(floats.y).pushVertexFloat(floats.z); //0
	pushVertexFloat(floats.x + 1.0f).pushVertexFloat(floats.y).pushVertexFloat(floats.z + 1.0f); //4
	pushVertexFloat(floats.x).pushVertexFloat(floats.y).pushVertexFloat(floats.z + 1.0f); //3
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
	pushLighting(0.8f).pushLighting(0.8f).pushLighting(0.8f).pushLighting(0.8f);

	pushNewIndices(size);
}

void ChunkMesh::pushSouth(Vector3i loc)
{
	int size{ static_cast<int>(m_Vertices.size()) / 3 };
	glm::vec3 floats{ static_cast<float>(loc.x), static_cast<float>(loc.y) , static_cast<float>(loc.z) };
	pushVertexFloat(floats.x).pushVertexFloat(floats.y).pushVertexFloat(floats.z + 1.0f); //3
	pushVertexFloat(floats.x).pushVertexFloat(floats.y).pushVertexFloat(floats.z); //0
	pushVertexFloat(floats.x).pushVertexFloat(floats.y + 1.0f).pushVertexFloat(floats.z + 1.0f); //6
	pushVertexFloat(floats.x).pushVertexFloat(floats.y + 1.0f).pushVertexFloat(floats.z); //2

	pushLighting(0.8f).pushLighting(0.8f).pushLighting(0.8f).pushLighting(0.8f);

	pushNewIndices(size);
}

void ChunkMesh::pushEast(Vector3i loc)
{
	int size{ static_cast<int>(m_Vertices.size()) / 3 };
	glm::vec3 floats{ static_cast<float>(loc.x), static_cast<float>(loc.y) , static_cast<float>(loc.z) };
	pushVertexFloat(floats.x + 1.0f).pushVertexFloat(floats.y).pushVertexFloat(floats.z + 1.0f); //4
	pushVertexFloat(floats.x).pushVertexFloat(floats.y).pushVertexFloat(floats.z + 1.0f); //3
	pushVertexFloat(floats.x + 1.0f).pushVertexFloat(floats.y + 1.0f).pushVertexFloat(floats.z + 1.0f); //7
	pushVertexFloat(floats.x).pushVertexFloat(floats.y + 1.0f).pushVertexFloat(floats.z + 1.0f); //6
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

void ChunkMesh::clear()
{
	m_Vertices.clear();
	m_TexCoords.clear();
	m_Lighting.clear();
	m_Indices.clear();

	glDeleteVertexArrays(1, &m_RenderData.vao);

	glDeleteBuffers(1, &m_RenderData.vbo);
	glDeleteBuffers(1, &m_RenderData.tbo);
	glDeleteBuffers(1, &m_RenderData.ebo);
	glDeleteBuffers(1, &m_RenderData.lbo);

	hasValidObjects = false;
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

	//float* dataArray{ new float[data.size()]{} };
	//for (int i{}; i < data.size(); ++i)
	//{
	//	dataArray[i] = data[i];
	//}

	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
	glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//delete[] dataArray;
}

void ChunkMesh::storeIndices(std::vector<int>& data)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RenderData.ebo);

	//int* dataArray{ new int[data.size()]{} };
	//for (int i{}; i < data.size(); ++i)
	//{
	//	dataArray[i] = data[i];
	//}
	
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.size() * sizeof(int), &data[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	m_RenderData.indexCount = static_cast<int>(data.size());
	//delete[] dataArray;
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