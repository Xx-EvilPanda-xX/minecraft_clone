#ifndef CHUNK_MESH_H
#define CHUNK_MESH_H

#include <vector>
#include "../Math/Vector3i.h"
#include "../Math/Vector2i.h"
#include "../Render/RenderData.h"

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
	//temporary
	bool m_TempHasAddedFace;

	Vector2i m_Pos;

	void storeBuffer(int index, int size, int buffer, std::vector<float>& data);

	void storeIndices(std::vector<int>& data);

public:
	ChunkMesh(Vector2i pos);

	~ChunkMesh();

	void addFace(Vector3i loc, Face face);

	void toBuffers();

	void enableAttribs() const;

	void disableAttribs() const;

	const std::vector<float>& getVertices() const;

	const std::vector<float>& getTexCoords() const;

	const std::vector<int>& getIndices() const;

	const RenderData& getRenderData() const;

	Vector2i getPostion() const;
};

#endif

