#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <unordered_map>
#include "Chunk.h"
#include "../Gen/TerrainGenerator.h"
#include "../Render/Renderer.h"
#include "../Render/Shader.h"
#include "ChunkManager.h"
#include "../Player/Player.h"

constexpr int genInterval{ 0 };

namespace std
{
	template<>
	struct hash<Vector2i>
	{
		size_t operator()(const Vector2i& key) const
		{
			return hash<int>{}(key.x) ^ hash<int>{}(key.y);
		}
	};
}

class World
{
private:
	std::unordered_map<Vector2i, Chunk*> m_Chunks;
	TerrainGenerator m_WorldGen;
	Shader m_Shader;
	ChunkManager& m_Manager;
	Player m_Player;

	Vector3i getPlayerBlockPos(const Camera& camera);

public:
	World(Shader shader, Player player, ChunkManager& manager);

	void worldRender(const Camera& camera);
	
	void worldUpdate(const Camera& camera, bool deletePass);

	void genPass();

	void destroyPass(Vector2i playerPos);

	void buildPass();

	void reloadChunks(const Camera& camera);

	std::unordered_map<Vector2i, Chunk*>& getChunks();

	ChunkManager& getManager();

	Player& getPlayer();
};

#endif
