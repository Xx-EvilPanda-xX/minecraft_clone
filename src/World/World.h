#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <mutex>
#include <unordered_map>
#include "Chunk.h"
#include "../Gen/TerrainGenerator.h"
#include "../Render/Renderer.h"
#include "../Render/Shader.h"
#include "ChunkManager.h"
#include "../Player/Player.h"
#include "../Input/EventHandler.h"

constexpr int genInterval{ 0 };

void chunker(World* world);

class World
{
private:
	std::mutex m_ChunksMutex;
	std::vector<Chunk*> m_Chunks;

	std::mutex m_UploadPendingMutex;
	std::vector<Chunk*> m_UploadPending;

	std::mutex m_DestroyQueueMutex;
	std::vector<Chunk*> m_DestroyQueue;

	std::mutex m_BufferDestroyQueueMutex;
	std::vector<unsigned int> m_BufferDestroyQueue;

	std::vector<std::thread> m_ChunkSaverThreads;

	TerrainGenerator m_WorldGen;
	Shader m_Shader;
	ChunkManager m_Manager;
	Player m_Player;

	size_t m_LastBlockQueueSize;
	bool m_ResetBuildVars;
	bool m_ShouldCloseChunkerThread;

	void uploadAll();

	void destroyAllBuffers();

	void updateSkyColor(Vector3i playerPos);

public:
	std::mutex m_GlobalPlayerLocationMutex;
	glm::vec3 m_GlobalPlayerLocation;

	std::mutex m_AllowChunkDestructionMutex;
	bool m_AllowChunkDestruction;

	static std::string s_WorldSaveDir;
	static bool s_ShouldSaveChunks;

	static void getChunkSaveInput();

	World(Shader shader, Keyboard& keyboard);

	~World();

	void render(const Window& window);
	
	void update();

	void genPass();

	void destroyPass(Vector2i playerChunkPos);

	void disallowChunkDestruction();

	void allowChunkDestruction();

	void buildPass();

	void rebuildChunks(const Camera& camera);

	int getChunkIndex(Vector2i chunkPos);

	void saveLevelData(EventHandler& handler);

	void loadLevelData(EventHandler& handler);

	std::mutex& getChunksMutex();

	std::vector<Chunk*>& getChunks();

	std::vector<Chunk*>& getUploadPending();

	ChunkManager& getManager();

	Player& getPlayer();

	bool shouldCloseChunkerThread();

	void setShouldCloseChunkerThread(bool close);

	void placeQueueBlocks();
};

#endif
