#include <cmath>
#include <glad/glad.h>
#include "ChunkMesh.h"
#include "Chunk.h"

ChunkMesh::ChunkMesh(Vector2i pos, Shader& shader)
	: m_Pos{ pos },
	m_RenderData{ 0, 0, 0, 0, 0, 0, &shader },
	m_FaceRatio{ 1.0f / facesPerRow }
{
	hasValidObjects = false;
}

Texture ChunkMesh::s_TexAltas{ Texture{} };
BlockType ChunkMesh::s_AtlasIndices[]{ BlockType::Grass, BlockType::Stone, BlockType::Dirt,
									BlockType::CobbleStone, BlockType::Wood, BlockType::Leaves,
									BlockType::Glass, BlockType::CraftingTable, BlockType::Planks,
									BlockType::DiamondBlock, BlockType::Water, BlockType::Sand };

void ChunkMesh::createTextureAtlas(const char* path)
{
	s_TexAltas = Texture{ path };
}

void ChunkMesh::addFace(Vector3i loc, Block block, Face face)
{
	m_CurrentFace = face;

	switch (face)
	{
	case Face::Up:
		pushUp(loc, block);
		break;

	case Face::Down:
		pushDown(loc);
		break;

	case Face::North:
		pushNorth(loc, block);
		break;

	case Face::South:
		pushSouth(loc, block);
		break;

	case Face::East:
		pushEast(loc, block);
		break;

	case Face::West:
		pushWest(loc, block);
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
	for (int i{}; i < BlockType::NUM_BLOCKS; ++i)
	{
		if (s_AtlasIndices[i] == block)
		{
			index = i;
			break;
		}
	}

	float fullIndex{ index * (6 * m_FaceRatio) };

	//extract integer part and decimal part of float
	float y{ static_cast<float>(static_cast<int>(fullIndex)) };
	float x{ fullIndex - y };
	y *= m_FaceRatio;
	
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
			glm::vec2 faceStartCoords{ startCoord + glm::vec2{ m_FaceRatio, 0.0f } };
			checkTexCoordBounds(faceStartCoords);
			return getTexCoordsFromStartPos(faceStartCoords);
		}
		case Face::East:
		{
			glm::vec2 faceStartCoords{ startCoord + glm::vec2{ m_FaceRatio * 2.0f, 0.0f } };
			checkTexCoordBounds(faceStartCoords);
			return getTexCoordsFromStartPos(faceStartCoords);
		}
		case Face::South:
		{
			glm::vec2 faceStartCoords{ startCoord + glm::vec2{ m_FaceRatio * 3.0f, 0.0f } };
			checkTexCoordBounds(faceStartCoords);
			return getTexCoordsFromStartPos(faceStartCoords);
		}
		case Face::West:
		{
			glm::vec2 faceStartCoords{ startCoord + glm::vec2{ m_FaceRatio * 4.0f, 0.0f } };
			checkTexCoordBounds(faceStartCoords);
			return getTexCoordsFromStartPos(faceStartCoords);
		}
		case Face::Down:
		{
			glm::vec2 faceStartCoords{ startCoord + glm::vec2{ m_FaceRatio * 5.0f, 0.0f } };
			checkTexCoordBounds(faceStartCoords);
			return getTexCoordsFromStartPos(faceStartCoords);
		}
	}
}

void ChunkMesh::checkTexCoordBounds(glm::vec2& faceStartCoords)
{
	if (faceStartCoords.x >= 1.0f)
	{
		faceStartCoords.y += m_FaceRatio;
		faceStartCoords.x = faceStartCoords.x - 1.0f;
	}
}

float* ChunkMesh::getTexCoordsFromStartPos(glm::vec2 startPos)
{
	float* coords{ new float[8]{} };
	
	coords[0] = startPos.x + m_FaceRatio;
	coords[1] = startPos.y + m_FaceRatio;

	coords[2] = startPos.x;
	coords[3] = startPos.y + m_FaceRatio;

	coords[4] = startPos.x + m_FaceRatio;
	coords[5] = startPos.y;

	coords[6] = startPos.x;
	coords[7] = startPos.y;

	return coords;
}

void ChunkMesh::pushUp(Vector3i loc, Block block)
{
	int size{ static_cast<int>(m_Vertices.size()) / 3 };
	glm::vec3 floats{ static_cast<float>(loc.x), static_cast<float>(loc.y) , static_cast<float>(loc.z) };
	float height{ block.isSurface() ? 0.9f : 1.0f };

	pushVertexFloat(floats.x).pushVertexFloat(floats.y + height).pushVertexFloat(floats.z); //2
	pushVertexFloat(floats.x + 1.0f).pushVertexFloat(floats.y + height).pushVertexFloat(floats.z); //5
	pushVertexFloat(floats.x).pushVertexFloat(floats.y + height).pushVertexFloat(floats.z + 1.0f); //6
	pushVertexFloat(floats.x + 1.0f).pushVertexFloat(floats.y + height).pushVertexFloat(floats.z + 1.0f); //7

	for (int i{}; i < 4; ++i)
	{
		pushLighting(upAmbient);
	}

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

	for (int i{}; i < 4; ++i)
	{
		pushLighting(downAmbient);
	}

	pushNewIndices(size);
}

void ChunkMesh::pushNorth(Vector3i loc, Block block)
{
	int size{ static_cast<int>(m_Vertices.size()) / 3 };
	glm::vec3 floats{ static_cast<float>(loc.x), static_cast<float>(loc.y) , static_cast<float>(loc.z) };
	float height{ block.isSurface() ? 0.9f : 1.0f };

	pushVertexFloat(floats.x + 1.0f).pushVertexFloat(floats.y).pushVertexFloat(floats.z); //1
	pushVertexFloat(floats.x + 1.0f).pushVertexFloat(floats.y).pushVertexFloat(floats.z + 1.0f); //4
	pushVertexFloat(floats.x + 1.0f).pushVertexFloat(floats.y + height).pushVertexFloat(floats.z); //5
	pushVertexFloat(floats.x + 1.0f).pushVertexFloat(floats.y + height).pushVertexFloat(floats.z + 1.0f); //7
	
	for (int i{}; i < 4; ++i)
	{
		pushLighting(northAmbient);
	}

	pushNewIndices(size);
}

void ChunkMesh::pushSouth(Vector3i loc, Block block)
{
	int size{ static_cast<int>(m_Vertices.size()) / 3 };
	glm::vec3 floats{ static_cast<float>(loc.x), static_cast<float>(loc.y) , static_cast<float>(loc.z) };

	float height{ block.isSurface() ? 0.9f : 1.0f };

	pushVertexFloat(floats.x).pushVertexFloat(floats.y).pushVertexFloat(floats.z + 1.0f); //3
	pushVertexFloat(floats.x).pushVertexFloat(floats.y).pushVertexFloat(floats.z); //0
	pushVertexFloat(floats.x).pushVertexFloat(floats.y + height).pushVertexFloat(floats.z + 1.0f); //6
	pushVertexFloat(floats.x).pushVertexFloat(floats.y + height).pushVertexFloat(floats.z); //2

	for (int i{}; i < 4; ++i)
	{
		pushLighting(southAmbient);
	}

	pushNewIndices(size);
}

void ChunkMesh::pushEast(Vector3i loc, Block block)
{
	int size{ static_cast<int>(m_Vertices.size()) / 3 };
	glm::vec3 floats{ static_cast<float>(loc.x), static_cast<float>(loc.y) , static_cast<float>(loc.z) };
	float height{ block.isSurface() ? 0.9f : 1.0f };

	pushVertexFloat(floats.x + 1.0f).pushVertexFloat(floats.y).pushVertexFloat(floats.z + 1.0f); //4
	pushVertexFloat(floats.x).pushVertexFloat(floats.y).pushVertexFloat(floats.z + 1.0f); //3
	pushVertexFloat(floats.x + 1.0f).pushVertexFloat(floats.y + height).pushVertexFloat(floats.z + 1.0f); //7
	pushVertexFloat(floats.x).pushVertexFloat(floats.y + height).pushVertexFloat(floats.z + 1.0f); //6

	for (int i{}; i < 4; ++i)
	{
		pushLighting(eastAmbient);
	}

	pushNewIndices(size);
}

void ChunkMesh::pushWest(Vector3i loc, Block block)
{
	glm::vec3 floats{ static_cast<float>(loc.x), static_cast<float>(loc.y) , static_cast<float>(loc.z) };
	int size{ static_cast<int>(m_Vertices.size()) / 3 };
	float height{ block.isSurface() ? 0.9f : 1.0f };

	pushVertexFloat(floats.x).pushVertexFloat(floats.y).pushVertexFloat(floats.z); //0
	pushVertexFloat(floats.x + 1.0f).pushVertexFloat(floats.y).pushVertexFloat(floats.z); //1
	pushVertexFloat(floats.x).pushVertexFloat(floats.y + height).pushVertexFloat(floats.z); //2
	pushVertexFloat(floats.x + 1.0f).pushVertexFloat(floats.y + height).pushVertexFloat(floats.z); //5

	for (int i{}; i < 4; ++i)
	{
		pushLighting(westAmbient);
	}
	
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
	m_Vertices.resize(0);
	m_TexCoords.resize(0);
	m_Lighting.resize(0);
	m_Indices.resize(0);

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

	if (!m_Vertices.empty())
	{
		storeFloatBuffer(0, 3, m_RenderData.vbo, m_Vertices);
		storeFloatBuffer(1, 2, m_RenderData.tbo, m_TexCoords);
		storeFloatBuffer(2, 1, m_RenderData.lbo, m_Lighting);

		storeIndices(m_Indices);
	}

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