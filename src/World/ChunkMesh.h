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

constexpr double upAmbient{ 1.0 };
constexpr double northAmbient{ 0.9 };
constexpr double eastAmbient{ 0.8 };
constexpr double southAmbient{ 0.9 };
constexpr double westAmbient{ 0.8 };
constexpr double downAmbient{ 0.6 };

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
	std::vector<double> m_Vertices;
	std::vector<double> m_TexCoords;
	std::vector<double> m_Lighting;
	std::vector<int> m_Indices;
	bool hasValidObjects;
	static Texture s_TexAltas;
	static BlockType s_AtlasIndices[];
	const double m_FaceRatio;
	Vector2i m_Pos;

	void storeDoubleBuffer(int index, int size, int buffer, const std::vector<double>& data);

	void storeIndices(std::vector<int>& data);

	ChunkMesh& pushVertexdouble(double f);

	ChunkMesh& pushTexdouble(double f);

	ChunkMesh& pushLighting(double f);

	ChunkMesh& pushIndex(int i);

	void pushUp(Vector3i loc, Block block);

	void pushDown(Vector3i loc);

	void pushNorth(Vector3i loc, Block block);

	void pushSouth(Vector3i loc, Block block);

	void pushEast(Vector3i loc, Block block);

	void pushWest(Vector3i loc, Block block);

	void pushNewIndices(int size);

	double* calcTexCoords(BlockType block, Face face);

	double* getTexCoordsFromStartPos(glm::dvec2 startPos);

	void checkTexCoordBounds(glm::dvec2& faceStartCoords);

	void deleteBuffers();

public:
	ChunkMesh(Vector2i pos, Shader& shader);

	static void createTextureAtlas(const char* path);

	void addFace(Vector3i loc, Block block, Face face);

	void toBuffers();

	void clear();

	void enableAttribs() const;

	void disableAttribs() const;

	const std::vector<double>& getVertices() const;

	const std::vector<double>& getTexCoords() const;

	const std::vector<int>& getIndices() const;

	const RenderData& getRenderData() const;

	const Vector2i& getPostion() const;
};

#endif

