#include <cmath>
#include <glad/glad.h>
#include "ChunkMesh.h"
#include "Chunk.h"

ChunkMesh::ChunkMesh(Vector2i pos, Shader& shader,
					std::pair<std::mutex&, std::vector<unsigned int>&> bufferDestroyQueue)
	: m_Pos{ pos },
	m_RenderData{ 0, 0, 0, 0, 0, 0, &shader },
	m_FaceRatio{ 1.0f / facesPerRow },
	m_BufferDestroyQueueMutex{ bufferDestroyQueue.first },
	m_BufferDestroyQueue{ bufferDestroyQueue.second }
{
	m_HasValidObjects = false;
}

Texture ChunkMesh::s_TexAltas{ Texture{} };
BlockType ChunkMesh::s_AtlasIndices[]{ BlockType::Grass, BlockType::Stone, BlockType::Dirt,
									BlockType::CobbleStone, BlockType::Wood, BlockType::PalmLeaves,
									BlockType::Glass, BlockType::CraftingTable, BlockType::Planks,
									BlockType::DiamondBlock, BlockType::Water, BlockType::Sand,
									BlockType::Gravel, BlockType::OakLeaves, BlockType::Cactus,
									BlockType::Snow, BlockType::Flowers, BlockType::TallGrass,
									BlockType::Shrub, BlockType::Rose, BlockType::RedWool, 
									BlockType::OrangeWool, BlockType::YellowWool, BlockType::GreenWool,
									BlockType::SkyBlueWool, BlockType::BlueWool, BlockType::PurpleWool,
									BlockType::PinkWool, BlockType::BrownWool, BlockType::BlackWool };
	
void ChunkMesh::createTextureAtlas(const char* path)
{
	s_TexAltas = Texture{ path, false };
}

void ChunkMesh::addFace(Vector3i loc, Block block, Face face, bool copyBothSides)
{
	glm::vec3 floats{ static_cast<float>(loc.x), static_cast<float>(loc.y) , static_cast<float>(loc.z) };
	float height{ block.isSurface() ? 0.9f : 1.0f };

	if (block.isFoliageMesh())
	{
		pushSmallFoliage(floats);

		//used as a toggle from 0 to 1 (Up and North)
		bool face{};
		for (int i{}; i < 4; ++i)
		{
			float* tex{ calcTexCoords(block.getType(), static_cast<Face>(face)) };
			face = !face;
			for (int i{}; i < 8; ++i)
			{
				m_TexCoords.push_back(tex[i]);
			}

			delete[] tex;
		}
	}
	else
	{
		switch (face)
		{
		case Face::Up:
			pushUp(floats, height, copyBothSides);
			break;

		case Face::Down:
			pushDown(floats, copyBothSides);
			break;

		case Face::North:
			pushNorth(floats, height, copyBothSides);
			break;

		case Face::South:
			pushSouth(floats, height, copyBothSides);
			break;

		case Face::East:
			pushEast(floats, height, copyBothSides);
			break;

		case Face::West:
			pushWest(floats, height, copyBothSides);
			break;
		}

		for (int i{}; i < (copyBothSides ? 2 : 1); ++i)
		{
			float* tex{ calcTexCoords(block.getType(), face) };
			for (int i{}; i < 8; ++i)
			{
				m_TexCoords.push_back(tex[i]);
			}

			delete[] tex;
		}
	}
}

float* ChunkMesh::calcTexCoords(BlockType block, Face face)
{
	float index{};
	for (int i{}; i < BlockType::NUM_BLOCKS; ++i)
	{
		if (s_AtlasIndices[i] == block)
		{
			index = static_cast<float>(i);
			break;
		}
	}

	float fullIndex{ index * (6 * m_FaceRatio) };

	//extract integer part and decimal part of float
	float y{ static_cast<float>(static_cast<int>(fullIndex)) };
	float x{ fullIndex - y };
	y *= m_FaceRatio;
	
	glm::vec2 startCoords{ x, y };
	glm::vec2 faceCoords{};

	switch (face)
	{
		case Face::Up:
			faceCoords = { startCoords };
			break;

		case Face::North:
			faceCoords = { startCoords + glm::vec2{ m_FaceRatio, 0.0f } };
			break;

		case Face::East:
			faceCoords = { startCoords + glm::vec2{ m_FaceRatio * 2.0f, 0.0f } };
			break;

		case Face::South:
			faceCoords = { startCoords + glm::vec2{ m_FaceRatio * 3.0f, 0.0f } };
			break;

		case Face::West:
			faceCoords = { startCoords + glm::vec2{ m_FaceRatio * 4.0f, 0.0f } };
			break;

		case Face::Down:
			faceCoords = { startCoords + glm::vec2{ m_FaceRatio * 5.0f, 0.0f } };
			break;
	}

	checkTexCoordBounds(faceCoords);
	return getTexCoordsFromStartPos(faceCoords);
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

void ChunkMesh::pushUp(glm::vec3& floats, float height, bool copyBothSides)
{
	constexpr float add{ 1.0f };

	for (int i{}; i < (copyBothSides ? 2 : 1); ++i)
	{
		if (i == 0)
			pushCCWIndices();
		else if (i == 1)
			pushCWIndices();

		vertex(floats.x, floats.y + height, floats.z); //2
		vertex(floats.x + add, floats.y + height, floats.z); //5
		vertex(floats.x, floats.y + height, floats.z + add); //6
		vertex(floats.x + add, floats.y + height, floats.z + add); //7

		for (int i{}; i < 4; ++i)
		{
			pushLighting(upAmbient);
		}
	}
}

void ChunkMesh::pushDown(glm::vec3& floats, bool copyBothSides)
{
	constexpr float add{ 1.0f };

	for (int i{}; i < (copyBothSides ? 2 : 1); ++i)
	{
		if (i == 0)
			pushCCWIndices();
		else if (i == 1)
			pushCWIndices();

		vertex(floats.x + add, floats.y, floats.z); //1
		vertex(floats.x, floats.y, floats.z); //0
		vertex(floats.x + add, floats.y, floats.z + add); //4
		vertex(floats.x, floats.y, floats.z + add); //3

		for (int i{}; i < 4; ++i)
		{
			pushLighting(downAmbient);
		}
	}
}

void ChunkMesh::pushNorth(glm::vec3& floats, float height, bool copyBothSides)
{
	constexpr float add{ 1.0f };

	for (int i{}; i < (copyBothSides ? 2 : 1); ++i)
	{
		if (i == 0)
			pushCCWIndices();
		else if (i == 1)
			pushCWIndices();

		vertex(floats.x + add, floats.y, floats.z); //1
		vertex(floats.x + add, floats.y, floats.z + add); //4
		vertex(floats.x + add, floats.y + height, floats.z); //5
		vertex(floats.x + add, floats.y + height, floats.z + add); //7

		for (int i{}; i < 4; ++i)
		{
			pushLighting(northAmbient);
		}
	}
}

void ChunkMesh::pushSouth(glm::vec3& floats, float height, bool copyBothSides)
{
	constexpr float add{ 1.0f };

	for (int i{}; i < (copyBothSides ? 2 : 1); ++i)
	{
		if (i == 0)
			pushCCWIndices();
		else if (i == 1)
			pushCWIndices();

		vertex(floats.x, floats.y, floats.z + add); //3
		vertex(floats.x, floats.y, floats.z); //0
		vertex(floats.x, floats.y + height, floats.z + add); //6
		vertex(floats.x, floats.y + height, floats.z); //2

		for (int i{}; i < 4; ++i)
		{
			pushLighting(southAmbient);
		}
	}
}

void ChunkMesh::pushEast(glm::vec3& floats, float height, bool copyBothSides)
{
	constexpr float add{ 1.0f };

	for (int i{}; i < (copyBothSides ? 2 : 1); ++i)
	{
		if (i == 0)
			pushCCWIndices();
		else if (i == 1)
			pushCWIndices();

		vertex(floats.x + add, floats.y, floats.z + add); //4
		vertex(floats.x, floats.y, floats.z + add); //3
		vertex(floats.x + add, floats.y + height, floats.z + add); //7
		vertex(floats.x, floats.y + height, floats.z + add); //6

		for (int i{}; i < 4; ++i)
		{
			pushLighting(eastAmbient);
		}
	}
}

void ChunkMesh::pushWest(glm::vec3& floats, float height, bool copyBothSides)
{
	constexpr float add{ 1.0f };

	for (int i{}; i < (copyBothSides ? 2 : 1); ++i)
	{
		if (i == 0)
			pushCCWIndices();
		else if (i == 1)
			pushCWIndices();

		vertex(floats.x, floats.y, floats.z); //0
		vertex(floats.x + add, floats.y, floats.z); //1
		vertex(floats.x, floats.y + height, floats.z); //2
		vertex(floats.x + add, floats.y + height, floats.z); //5

		for (int i{}; i < 4; ++i)
		{
			pushLighting(westAmbient);
		}
	}
}

void ChunkMesh::pushSmallFoliage(glm::vec3& floats)
{
	constexpr float sqrt2{ 1.41421356f };
	constexpr float add{ (sqrt2 - 1.0f) / (2.0f * sqrt2) };

	pushCCWIndices();
	vertex(floats.x + add, floats.y, floats.z + add);
	vertex(floats.x + (1.0f - add), floats.y, floats.z + (1.0f - add));
	vertex(floats.x + add, floats.y + 1.0f, floats.z + add);
	vertex(floats.x + (1.0f - add), floats.y + 1.0f, floats.z + (1.0f - add));

	for (int i{}; i < 4; ++i)
	{
		pushLighting(northAmbient);
	}

	pushCWIndices();
	vertex(floats.x + add, floats.y, floats.z + add);
	vertex(floats.x + (1.0f - add), floats.y, floats.z + (1.0f - add));
	vertex(floats.x + add, floats.y + 1.0f, floats.z + add);
	vertex(floats.x + (1.0f - add), floats.y + 1.0f, floats.z + (1.0f - add));

	for (int i{}; i < 4; ++i)
	{
		pushLighting(westAmbient);
	}

	pushCCWIndices();
	vertex(floats.x + add, floats.y, floats.z + (1.0f - add));
	vertex(floats.x + (1.0f - add), floats.y, floats.z + add);
	vertex(floats.x + add, floats.y + 1.0f, floats.z + (1.0f - add));
	vertex(floats.x + (1.0f - add), floats.y + 1.0f, floats.z + add);

	for (int i{}; i < 4; ++i)
	{
		pushLighting(westAmbient);
	}

	pushCWIndices();
	vertex(floats.x + add, floats.y, floats.z + (1.0f - add));
	vertex(floats.x + (1.0f - add), floats.y, floats.z + add);
	vertex(floats.x + add, floats.y + 1.0f, floats.z + (1.0f - add));
	vertex(floats.x + (1.0f - add), floats.y + 1.0f, floats.z + add);

	for (int i{}; i < 4; ++i)
	{
		pushLighting(westAmbient);
	}
}

void ChunkMesh::pushCCWIndices()
{
	int size{ static_cast<int>(m_Vertices.size() / 3) };
	int s0{ size + 0 };
	int s1{ size + 1 };
	int s2{ size + 2 };
	int s3{ size + 3 };

	pushIndex(s1).pushIndex(s2).pushIndex(s3).pushIndex(s0).pushIndex(s2).pushIndex(s1);
}

void ChunkMesh::pushCWIndices()
{
	int size{ static_cast<int>(m_Vertices.size() / 3) };
	int s0{ size + 0 };
	int s1{ size + 1 };
	int s2{ size + 2 };
	int s3{ size + 3 };

	pushIndex(s1).pushIndex(s3).pushIndex(s2).pushIndex(s0).pushIndex(s1).pushIndex(s2);
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

void ChunkMesh::vertex(float f1, float f2, float f3)
{
	pushVertexFloat(f1);
	pushVertexFloat(f2);
	pushVertexFloat(f3);
}

void ChunkMesh::clear()
{
	m_Vertices.resize(0);
	m_TexCoords.resize(0);
	m_Lighting.resize(0);
	m_Indices.resize(0);

	if (m_HasValidObjects)
		deleteBuffers();

	m_HasValidObjects = false;
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
	std::lock_guard<std::mutex> lock{ m_BufferDestroyQueueMutex };
	m_BufferDestroyQueue.push_back(m_RenderData.vao);
	m_BufferDestroyQueue.push_back(m_RenderData.vbo);
	m_BufferDestroyQueue.push_back(m_RenderData.tbo);
	m_BufferDestroyQueue.push_back(m_RenderData.lbo);
	m_BufferDestroyQueue.push_back(m_RenderData.ebo);
}

void ChunkMesh::toBuffers()
{
	if (m_HasValidObjects)
		deleteBuffers();

	glGenVertexArrays(1, &m_RenderData.vao);

	glGenBuffers(1, &m_RenderData.vbo);
	glGenBuffers(1, &m_RenderData.tbo);
	glGenBuffers(1, &m_RenderData.lbo);
	glGenBuffers(1, &m_RenderData.ebo);

	if (!m_Vertices.empty())
	{
		storeFloatBuffer(0, 3, m_RenderData.vbo, m_Vertices);
		storeFloatBuffer(1, 2, m_RenderData.tbo, m_TexCoords);
		storeFloatBuffer(2, 1, m_RenderData.lbo, m_Lighting);

		storeIndices(m_Indices);
	}

	m_RenderData.texture = s_TexAltas.getId();

	m_HasValidObjects = true;

	//don't waste memory saving vertex buffers in cpu memory
	//m_Vertices.resize(0);
	//m_TexCoords.resize(0);
	//m_Lighting.resize(0);
	//m_Indices.resize(0);
}

void ChunkMesh::storeFloatBuffer(int index, int size, int buffer, const std::vector<float>& data)
{
	glBindVertexArray(m_RenderData.vao);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void ChunkMesh::storeIndices(std::vector<int>& data)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RenderData.ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.size() * sizeof(int), data.data(), GL_STATIC_DRAW);
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

const Vector2i& ChunkMesh::getPostion() const
{
	return m_Pos;
}