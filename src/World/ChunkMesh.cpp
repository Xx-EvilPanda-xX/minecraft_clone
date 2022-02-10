#include <cmath>
#include <glad/glad.h>
#include "ChunkMesh.h"
#include "Chunk.h"

ChunkMesh::ChunkMesh(Vector2i pos, Shader& shader)
	: m_Pos{ pos },
	m_RenderData{ 0, 0, 0, 0, 0, 0, &shader },
	m_FaceRatio{ 1.0 / facesPerRow }
{
	hasValidObjects = false;
}

Texture ChunkMesh::s_TexAltas{ Texture{} };
BlockType ChunkMesh::s_AtlasIndices[]{ BlockType::Grass, BlockType::Stone, BlockType::Dirt,
									BlockType::CobbleStone, BlockType::Wood, BlockType::Leaves,
									BlockType::Glass, BlockType::CraftingTable, BlockType::Planks,
									BlockType::DiamondBlock, BlockType::Water, BlockType::Sand, BlockType::Gravel };

void ChunkMesh::createTextureAtlas(const char* path)
{
	s_TexAltas = Texture{ path };
}

void ChunkMesh::addFace(Vector3i loc, Block block, Face face)
{
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

	double* tex{ calcTexCoords(block.getType(), face) };
	for (int i{}; i < 8; ++i)
	{
		m_TexCoords.push_back(tex[i]);
	}

	delete[] tex;
}

double* ChunkMesh::calcTexCoords(BlockType block, Face face)
{
	double index{};
	for (int i{}; i < BlockType::NUM_BLOCKS; ++i)
	{
		if (s_AtlasIndices[i] == block)
		{
			index = i;
			break;
		}
	}

	double fullIndex{ index * (6 * m_FaceRatio) };

	//extract integer part and decimal part of double
	double y{ static_cast<double>(static_cast<int>(fullIndex)) };
	double x{ fullIndex - y };
	y *= m_FaceRatio;
	
	glm::dvec2 startCoord{ x, y };

	switch (face)
	{
		case Face::Up:
		{
			glm::dvec2 faceStartCoords{ startCoord };
			checkTexCoordBounds(faceStartCoords);
			return getTexCoordsFromStartPos(faceStartCoords);
		}	
		case Face::North:
		{
			glm::dvec2 faceStartCoords{ startCoord + glm::dvec2{ m_FaceRatio, 0.0 } };
			checkTexCoordBounds(faceStartCoords);
			return getTexCoordsFromStartPos(faceStartCoords);
		}
		case Face::East:
		{
			glm::dvec2 faceStartCoords{ startCoord + glm::dvec2{ m_FaceRatio * 2.0, 0.0 } };
			checkTexCoordBounds(faceStartCoords);
			return getTexCoordsFromStartPos(faceStartCoords);
		}
		case Face::South:
		{
			glm::dvec2 faceStartCoords{ startCoord + glm::dvec2{ m_FaceRatio * 3.0, 0.0 } };
			checkTexCoordBounds(faceStartCoords);
			return getTexCoordsFromStartPos(faceStartCoords);
		}
		case Face::West:
		{
			glm::dvec2 faceStartCoords{ startCoord + glm::dvec2{ m_FaceRatio * 4.0, 0.0 } };
			checkTexCoordBounds(faceStartCoords);
			return getTexCoordsFromStartPos(faceStartCoords);
		}
		case Face::Down:
		{
			glm::dvec2 faceStartCoords{ startCoord + glm::dvec2{ m_FaceRatio * 5.0, 0.0 } };
			checkTexCoordBounds(faceStartCoords);
			return getTexCoordsFromStartPos(faceStartCoords);
		}
	}
}

void ChunkMesh::checkTexCoordBounds(glm::dvec2& faceStartCoords)
{
	if (faceStartCoords.x >= 1.0)
	{
		faceStartCoords.y += m_FaceRatio;
		faceStartCoords.x = faceStartCoords.x - 1.0;
	}
}

double* ChunkMesh::getTexCoordsFromStartPos(glm::dvec2 startPos)
{
	double* coords{ new double[8]{} };
	
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
	glm::dvec3 doubles{ static_cast<double>(loc.x), static_cast<double>(loc.y) , static_cast<double>(loc.z) };
	double height{ block.isSurface() ? 0.9 : 1.0 };

	pushVertexdouble(doubles.x).pushVertexdouble(doubles.y + height).pushVertexdouble(doubles.z); //2
	pushVertexdouble(doubles.x + 1.0).pushVertexdouble(doubles.y + height).pushVertexdouble(doubles.z); //5
	pushVertexdouble(doubles.x).pushVertexdouble(doubles.y + height).pushVertexdouble(doubles.z + 1.0); //6
	pushVertexdouble(doubles.x + 1.0).pushVertexdouble(doubles.y + height).pushVertexdouble(doubles.z + 1.0); //7

	for (int i{}; i < 4; ++i)
	{
		pushLighting(upAmbient);
	}

	pushNewIndices(size);
}

void ChunkMesh::pushDown(Vector3i loc)
{
	int size{ static_cast<int>(m_Vertices.size()) / 3 };
	glm::dvec3 doubles{ static_cast<double>(loc.x), static_cast<double>(loc.y) , static_cast<double>(loc.z) };
	pushVertexdouble(doubles.x + 1.0).pushVertexdouble(doubles.y).pushVertexdouble(doubles.z); //1
	pushVertexdouble(doubles.x).pushVertexdouble(doubles.y).pushVertexdouble(doubles.z); //0
	pushVertexdouble(doubles.x + 1.0).pushVertexdouble(doubles.y).pushVertexdouble(doubles.z + 1.0); //4
	pushVertexdouble(doubles.x).pushVertexdouble(doubles.y).pushVertexdouble(doubles.z + 1.0); //3

	for (int i{}; i < 4; ++i)
	{
		pushLighting(downAmbient);
	}

	pushNewIndices(size);
}

void ChunkMesh::pushNorth(Vector3i loc, Block block)
{
	int size{ static_cast<int>(m_Vertices.size()) / 3 };
	glm::dvec3 doubles{ static_cast<double>(loc.x), static_cast<double>(loc.y) , static_cast<double>(loc.z) };
	double height{ block.isSurface() ? 0.9 : 1.0 };

	pushVertexdouble(doubles.x + 1.0).pushVertexdouble(doubles.y).pushVertexdouble(doubles.z); //1
	pushVertexdouble(doubles.x + 1.0).pushVertexdouble(doubles.y).pushVertexdouble(doubles.z + 1.0); //4
	pushVertexdouble(doubles.x + 1.0).pushVertexdouble(doubles.y + height).pushVertexdouble(doubles.z); //5
	pushVertexdouble(doubles.x + 1.0).pushVertexdouble(doubles.y + height).pushVertexdouble(doubles.z + 1.0); //7
	
	for (int i{}; i < 4; ++i)
	{
		pushLighting(northAmbient);
	}

	pushNewIndices(size);
}

void ChunkMesh::pushSouth(Vector3i loc, Block block)
{
	int size{ static_cast<int>(m_Vertices.size()) / 3 };
	glm::dvec3 doubles{ static_cast<double>(loc.x), static_cast<double>(loc.y) , static_cast<double>(loc.z) };

	double height{ block.isSurface() ? 0.9 : 1.0 };

	pushVertexdouble(doubles.x).pushVertexdouble(doubles.y).pushVertexdouble(doubles.z + 1.0); //3
	pushVertexdouble(doubles.x).pushVertexdouble(doubles.y).pushVertexdouble(doubles.z); //0
	pushVertexdouble(doubles.x).pushVertexdouble(doubles.y + height).pushVertexdouble(doubles.z + 1.0); //6
	pushVertexdouble(doubles.x).pushVertexdouble(doubles.y + height).pushVertexdouble(doubles.z); //2

	for (int i{}; i < 4; ++i)
	{
		pushLighting(southAmbient);
	}

	pushNewIndices(size);
}

void ChunkMesh::pushEast(Vector3i loc, Block block)
{
	int size{ static_cast<int>(m_Vertices.size()) / 3 };
	glm::dvec3 doubles{ static_cast<double>(loc.x), static_cast<double>(loc.y) , static_cast<double>(loc.z) };
	double height{ block.isSurface() ? 0.9 : 1.0 };

	pushVertexdouble(doubles.x + 1.0).pushVertexdouble(doubles.y).pushVertexdouble(doubles.z + 1.0); //4
	pushVertexdouble(doubles.x).pushVertexdouble(doubles.y).pushVertexdouble(doubles.z + 1.0); //3
	pushVertexdouble(doubles.x + 1.0).pushVertexdouble(doubles.y + height).pushVertexdouble(doubles.z + 1.0); //7
	pushVertexdouble(doubles.x).pushVertexdouble(doubles.y + height).pushVertexdouble(doubles.z + 1.0); //6

	for (int i{}; i < 4; ++i)
	{
		pushLighting(eastAmbient);
	}

	pushNewIndices(size);
}

void ChunkMesh::pushWest(Vector3i loc, Block block)
{
	glm::dvec3 doubles{ static_cast<double>(loc.x), static_cast<double>(loc.y) , static_cast<double>(loc.z) };
	int size{ static_cast<int>(m_Vertices.size()) / 3 };
	double height{ block.isSurface() ? 0.9 : 1.0 };

	pushVertexdouble(doubles.x).pushVertexdouble(doubles.y).pushVertexdouble(doubles.z); //0
	pushVertexdouble(doubles.x + 1.0).pushVertexdouble(doubles.y).pushVertexdouble(doubles.z); //1
	pushVertexdouble(doubles.x).pushVertexdouble(doubles.y + height).pushVertexdouble(doubles.z); //2
	pushVertexdouble(doubles.x + 1.0).pushVertexdouble(doubles.y + height).pushVertexdouble(doubles.z); //5

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

ChunkMesh& ChunkMesh::pushVertexdouble(double f)
{
	m_Vertices.push_back(f);
	return *this;
}

ChunkMesh& ChunkMesh::pushTexdouble(double f)
{
	m_TexCoords.push_back(f);
	return *this;
}

ChunkMesh& ChunkMesh::pushLighting(double f)
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

	deleteBuffers();

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

void ChunkMesh::deleteBuffers()
{
	glDeleteVertexArrays(1, &m_RenderData.vao);

	glDeleteBuffers(1, &m_RenderData.vbo);
	glDeleteBuffers(1, &m_RenderData.tbo);
	glDeleteBuffers(1, &m_RenderData.ebo);
	glDeleteBuffers(1, &m_RenderData.lbo);
}

void ChunkMesh::toBuffers()
{
	if (hasValidObjects)
	{
		deleteBuffers();
	}

	glGenVertexArrays(1, &m_RenderData.vao);

	glGenBuffers(1, &m_RenderData.vbo);
	glGenBuffers(1, &m_RenderData.tbo);
	glGenBuffers(1, &m_RenderData.ebo);
	glGenBuffers(1, &m_RenderData.lbo);

	if (!m_Vertices.empty())
	{
		storeDoubleBuffer(0, 3, m_RenderData.vbo, m_Vertices);
		storeDoubleBuffer(1, 2, m_RenderData.tbo, m_TexCoords);
		storeDoubleBuffer(2, 1, m_RenderData.lbo, m_Lighting);

		storeIndices(m_Indices);
	}

	m_RenderData.texture = s_TexAltas.getId();

	hasValidObjects = true;
}

void ChunkMesh::storeDoubleBuffer(int index, int size, int buffer, const std::vector<double>& data)
{
	glBindVertexArray(m_RenderData.vao);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	float* uploadableData{ new float[data.size()]{} };

	for (int i{}; i < data.size(); ++i)
	{
		uploadableData[i] = static_cast<float>(data[i]);
	}

	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &uploadableData[0], GL_STATIC_DRAW);
	glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void ChunkMesh::storeIndices(std::vector<int>& data)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RenderData.ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.size() * sizeof(int), &data[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	m_RenderData.indexCount = static_cast<int>(data.size());
}

const std::vector<double>& ChunkMesh::getVertices() const
{
	return m_Vertices;
}

const std::vector<double>& ChunkMesh::getTexCoords() const
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