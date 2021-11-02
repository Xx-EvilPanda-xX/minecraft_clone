#ifndef CHUNK_MESH_H
#define CHUNK_MESH_H

#include <vector>
#include <glm/vec3.hpp>
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

	void storeBuffer(int index, int size, int buffer, std::vector<float>& data);

	void storeIndices(std::vector<int>& data);

public:
	ChunkMesh();

	~ChunkMesh();

	void addFace(glm::vec3 loc, Face face);

	void toBuffers();

	void enableAttribs();

	void disableAttribs();

	const std::vector<float>& getVertices();

	const std::vector<float>& getTexCoords();

	const std::vector<int>& getIndices();

	const RenderData& getRenderData();
};

#endif

