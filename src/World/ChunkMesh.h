#ifndef CHUNK_MESH_H
#define CHUNK_MESH_H

#include <vector>
#include "../Math/Vector3i.h"
#include "../Math/Vector2i.h"
#include "../Render/RenderData.h"
#include "../Render/Shader.h"
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
	std::vector<int> m_Indices;
	bool hasValidObjects;

	Vector2i m_Pos;

	void storeBuffer(int index, int size, int buffer, std::vector<float>& data);

	void storeIndices(std::vector<int>& data);

	ChunkMesh& pushVertexFloat(float f);

	ChunkMesh& pushTexFloat(float f);

	ChunkMesh& pushInt(int i);

	void pushUp(Vector3i loc);

	void pushDown(Vector3i loc);

	void pushNorth(Vector3i loc);

	void pushSouth(Vector3i loc);

	void pushEast(Vector3i loc);

	void pushWest(Vector3i loc);

	void pushNewIndices(int size);

public:
	ChunkMesh(Vector2i pos, Shader& shader);

	~ChunkMesh();

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

