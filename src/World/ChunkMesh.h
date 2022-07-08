#ifndef CHUNK_MESH_H
#define CHUNK_MESH_H

#include <vector>
#include <mutex>
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
	North,
	South,
	East,
	West,
	Down
};

class ChunkMesh
{
private:
	RenderData m_RenderData;
	std::vector<float> m_Vertices;
	std::vector<float> m_TexCoords;
	std::vector<float> m_Lighting;
	std::vector<int> m_Indices;
	bool m_HasValidObjects;
	static Texture s_TexAltas;
	static BlockType s_AtlasIndices[];
	const float m_FaceRatio;
	Vector2i m_Pos;

	std::mutex& m_BufferDestroyQueueMutex;
	std::vector<unsigned int>& m_BufferDestroyQueue;

	void storeFloatBuffer(int index, int size, int buffer, const std::vector<float>& data);

	void storeIndices(std::vector<int>& data);

	void vertex(float f1, float f2, float f3);

	ChunkMesh& pushVertexFloat(float f);

	ChunkMesh& pushTexFloat(float f);

	ChunkMesh& pushLighting(float f);

	ChunkMesh& pushIndex(int i);

	void pushUp(glm::vec3& floats, float height, bool copyBothSides);

	void pushDown(glm::vec3& floats, bool copyBothSides);

	void pushNorth(glm::vec3& floats, float height, bool copyBothSides);

	void pushSouth(glm::vec3& floats, float height, bool copyBothSides);

	void pushEast(glm::vec3& floats, float height, bool copyBothSides);

	void pushWest(glm::vec3& floats, float height, bool copyBothSides);

	void pushSmallFoliage(glm::vec3& floats);

	void pushCCWIndices();

	void pushCWIndices();

	float* calcTexCoords(BlockType block, Face face);

	float* getTexCoordsFromStartPos(glm::vec2 startPos);

	void checkTexCoordBounds(glm::vec2& faceStartCoords);

	void deleteBuffers();

public:
	ChunkMesh(Vector2i pos, Shader& shader, std::pair<std::mutex&, std::vector<unsigned int>&> bufferDestroyQueue);

	static void createTextureAtlas(const char* path);

	void addFace(Vector3i loc, Block block, Face face, bool copyBothSides);

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

