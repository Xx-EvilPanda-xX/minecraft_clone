#ifndef CHUNK_MESH_H
#define CHUNK_MESH_H

#include <vector>
#include "../Math/Vector3i.h"
#include "../Math/Vector2i.h"
#include "../Render/RenderData.h"
#include "../Render/Shader.h"
#include "../Render/Texture.h"
#include "../World/Block.h"

constexpr int facesPerRow{ 16 };

constexpr float upAmbient{ 1.0f };
constexpr float northAmbient{ 0.9f };
constexpr float eastAmbient{ 0.8f };
constexpr float southAmbient{ 0.9f };
constexpr float westAmbient{ 0.8f };
constexpr float downAmbient{ 0.6f };

enum class Face
{
	Up,
	Down,
	North,
	South,
	East,
	West
};

class ChunkMesh
{
private:
	RenderData m_RenderData;
	std::vector<float> m_Vertices;
	std::vector<float> m_TexCoords;
	std::vector<float> m_Lighting;
	std::vector<int> m_Indices;
	bool hasValidObjects;
	static Texture s_TexAltas;
	static BlockType s_AtlasIndices[];
	const float m_FaceRatio;
	Vector2i m_Pos;

	void storeDoubleBuffer(int index, int size, int buffer, const std::vector<float>& data);

	void storeIndices(std::vector<int>& data);

	ChunkMesh& pushVertexdouble(float f);

	ChunkMesh& pushTexdouble(float f);

	ChunkMesh& pushLighting(float f);

	ChunkMesh& pushIndex(int i);

	void pushUp(Vector3i loc, Block block);

	void pushDown(Vector3i loc);

	void pushNorth(Vector3i loc, Block block);

	void pushSouth(Vector3i loc, Block block);

	void pushEast(Vector3i loc, Block block);

	void pushWest(Vector3i loc, Block block);

	void pushNewIndices(int size);

	float* calcTexCoords(BlockType block, Face face);

	float* getTexCoordsFromStartPos(glm::vec2 startPos);

	void checkTexCoordBounds(glm::vec2& faceStartCoords);

	void deleteBuffers();

public:
	ChunkMesh(Vector2i pos, Shader& shader);

	static void createTextureAtlas(const char* path);

	void addFace(Vector3i loc, Block block, Face face);

	void toBuffers();

	void clear();

	void enableAttribs() const;

	void disableAttribs() const;

	const std::vector<float>& getVertices() const;

	const std::vector<float>& getTexCoords() const;

	const std::vector<int>& getIndices() const;

	const RenderData& getRenderData() const;

	const Vector2i& getPostion() const;
};

#endif

