#ifndef CHUNK_MESH_H
#define CHUNK_MESH_H

#include <vector>
#include "../Math/Vector3i.h"
#include "../Math/Vector2i.h"
#include "../Render/RenderData.h"
#include "../Render/Shader.h"
#include "../Render/Texture.h"
#include "../World/Block.h"

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

	Vector2i m_Pos;

	void storeFloatBuffer(int index, int size, int buffer, const std::vector<float>& data);

	void storeIndices(std::vector<int>& data);

	ChunkMesh& pushVertexFloat(float f);

	ChunkMesh& pushTexFloat(float f);

	ChunkMesh& pushLighting(float f);

	ChunkMesh& pushIndex(int i);

	void pushUp(Vector3i loc);

	void pushDown(Vector3i loc);

	void pushNorth(Vector3i loc);

	void pushSouth(Vector3i loc);

	void pushEast(Vector3i loc);

	void pushWest(Vector3i loc);

	void pushNewIndices(int size);

	float* calcTexCoords(BlockType block, Face face);

	float* getTexCoordsFromStartPos(glm::vec2 startPos);

public:
	ChunkMesh(Vector2i pos, Shader& shader);

	static void createTextureAtlas(const char* path);

	void addFace(Vector3i loc, Block block, Face face);

	void toBuffers();

	void enableAttribs() const;

	void disableAttribs() const;

	const std::vector<float>& getVertices() const;

	const std::vector<float>& getTexCoords() const;

	const std::vector<int>& getIndices() const;

	const RenderData& getRenderData() const;

	const Vector2i& getPostion() const;
};

#endif

