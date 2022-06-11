#include "World.h"
#include <iostream>
#include <cmath>
#include <GLFW/glfw3.h>
#include "Chunk.h"
#include "../Math/Vector2i.h"
#include "../Render/Shader.h"
#include "ChunkManager.h"
#include "../Constants.h"

void chunker(World* world)
{
	static Vector2i lastPlayerChunkPos{};

	while (!world->shouldCloseChunkerThread())
	{
		Vector2i playerChunkPos{ Vector3i{ world->getPlayer().getCamera().getLocation() } };

		if (playerChunkPos != lastPlayerChunkPos)
			world->destroyPass(playerChunkPos);

		world->genPass();
		world->placeQueueBlocks();
		world->buildPass();
		lastPlayerChunkPos = playerChunkPos;
	}
}

//#define DEBUG
constexpr int chunkBuildsPerFrame{ constants::renderDistance / 2 };

World::World(Shader shader, Keyboard& keyboard)
	: m_Shader{ shader },
	m_Manager{ this },
	m_Player{ m_Manager, keyboard, constants::playerReach },
	m_WorldGen{ m_Manager }
{
	m_LastBlockQueueSize = 0;
	m_MoveCountDown = 100;
}

void World::update()
{
	Vector3i playerPos{ m_Player.getCamera().getLocation()};

	glClearColor(0.0f, 0.4f, 0.8f, 1.0f);
	if (playerPos.y < -5.0)
		glClearColor(0.0f, 0.0f, 0.1f, 1.0f);
	if (playerPos.y > 255.0)
		glClearColor(0.0f, 0.2f, 0.6f, 1.0f);
	if (playerPos.y > 512.0)
		glClearColor(0.0f, 0.0f, 0.2f, 1.0f);
	if (playerPos.y > 1024.0)
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	if (m_Manager.chunkExsists(playerPos) && m_MoveCountDown <= 0)
		m_Player.move();

	m_Manager.updateQueues(m_Player.getCamera());
	
	uploadAll();
	--m_MoveCountDown;
}

void World::render(const Window& window)
{
	Vector3i playerPos{ m_Player.getCamera().getLocation() };
	m_Shader.bind();
	m_Shader.setBool("playerUnderWater", m_Manager.getWorldBlock(playerPos).getType() == BlockType::Water);
	m_Shader.unbind();

	std::vector<Chunk*> copy{};

	{
		std::lock_guard<std::mutex> lock{ m_ChunksMutex };
		copy = m_Chunks;
	}

	Vector2i playerChunkPos{ Vector3i{ m_Player.getCamera().getLocation() } };
	for (int i{}; i < copy.size(); ++i)
	{
		Vector2i chunkLoc{ copy[i]->getLocation() };
		if (std::abs(chunkLoc.x - playerChunkPos.x) >= constants::hideDistance
			|| std::abs(chunkLoc.y - playerChunkPos.y) >= constants::hideDistance)
		{
			copy[i]->hide();
		}
		else
		{
			copy[i]->show();
		}
	}

	for (int i{}; i < copy.size(); ++i)
	{
		if (copy[i]->isBuilt() && !copy[i]->isHidden())
			Renderer::drawMesh(m_Player.getCamera(), copy[i]->getSolidMesh(), window);
	}

	if constexpr (constants::useTranslucentWater)
	{
		for (int i{}; i < copy.size(); ++i)
		{
			if (copy[i]->isBuilt() && !copy[i]->isHidden())
				Renderer::drawMesh(m_Player.getCamera(), copy[i]->getTranslucentMesh(), window);
		}
	}
}

void World::uploadAll()
{
	std::vector<Chunk*> copy{};

	{
		std::lock_guard<std::mutex> lock{ m_UploadPendingMutex };
		copy = m_UploadPending;
		m_UploadPending.clear();
	}

	for (int i{}; i < copy.size(); ++i)
	{
		copy[i]->finishBuilding();
	}
}

void World::genPass()
{
	bool empty{};

	{
		std::lock_guard<std::mutex> lock{ m_Manager.getGenQueueMutex() };
		empty = m_Manager.getGenQueue().empty();
	}
	
	if (!empty)
	{
		Vector2i chunkPos{};

		{
			std::lock_guard<std::mutex> lock{ m_Manager.getGenQueueMutex() };
			chunkPos = m_Manager.getGenQueue().back();
			m_Manager.getGenQueue().pop_back();
		}

#ifdef DEBUG
		if (chunkPos.x > 4 || chunkPos.x < -4 || chunkPos.y > 4 || chunkPos.y < -4)
			return;
#endif // DEBUG

		Chunk* chunk{ m_WorldGen.generateChunk(chunkPos, m_Shader) };

		std::lock_guard<std::mutex> lock{ m_ChunksMutex };
		m_Chunks.push_back(chunk);
	}
}

void World::destroyPass(Vector2i playerPos)
{
	std::lock_guard<std::mutex> lock{ m_ChunksMutex };

	auto findChunk{ 
		[this](Vector2i chunkLoc) {
			for (int i{}; i < m_Chunks.size(); ++i)
			{
				if (m_Chunks[i]->getLocation() == chunkLoc)
				{
					return m_Chunks[i];
				}
			}

			return (Chunk*) nullptr;
		}
	};

	for (int i{}; i < m_Chunks.size(); ++i)
	{
		Vector2i chunkLoc{ m_Chunks[i]->getLocation() };

		//+ 1 becuase otherwise chunks that had just been generated got deleted
		if (std::abs(chunkLoc.x - playerPos.x) > constants::renderDistance + 1 || std::abs(chunkLoc.y - playerPos.y) > constants::renderDistance + 1)
		{
			int index;

			if (m_Manager.isInBuildQueue(m_Chunks[i], index))
			{
				std::lock_guard<std::mutex> lock{ m_Manager.getBuildQueueMutex() };
				m_Manager.getBuildQueue().erase(m_Manager.getBuildQueue().begin() + index);
			}

			if (m_Manager.isInBuildQueue(findChunk(Vector2i{ chunkLoc.x + 1, chunkLoc.y }), index))
			{
				std::lock_guard<std::mutex> lock{ m_Manager.getBuildQueueMutex() };
				m_Manager.getBuildQueue().erase(m_Manager.getBuildQueue().begin() + index);
			}

			if (m_Manager.isInBuildQueue(findChunk(Vector2i{ chunkLoc.x, chunkLoc.y + 1 }), index))
			{
				std::lock_guard<std::mutex> lock{ m_Manager.getBuildQueueMutex() };
				m_Manager.getBuildQueue().erase(m_Manager.getBuildQueue().begin() + index);
			}

			if (m_Manager.isInBuildQueue(findChunk(Vector2i{ chunkLoc.x - 1, chunkLoc.y }), index))
			{
				std::lock_guard<std::mutex> lock{ m_Manager.getBuildQueueMutex() };
				m_Manager.getBuildQueue().erase(m_Manager.getBuildQueue().begin() + index);
			}

			if (m_Manager.isInBuildQueue(findChunk(Vector2i{ chunkLoc.x, chunkLoc.y - 1 }), index))
			{
				std::lock_guard<std::mutex> lock{ m_Manager.getBuildQueueMutex() };
				m_Manager.getBuildQueue().erase(m_Manager.getBuildQueue().begin() + index);
			}

			delete m_Chunks[i];
			m_Chunks.erase(m_Chunks.begin() + i);

			//erasing an element from the middle of the vec shifts all elements beyond that one down once, essentially incrementing i
			--i;
		}
	}
}

void World::buildPass()
{
	static int sectionPtr{};
	static int shouldGen{};

	if (m_ResetBuildVars)
	{
		sectionPtr = 0;
		shouldGen = 0;
		m_ResetBuildVars = false;
	}

	Chunk* currentChunk{ nullptr };

	{
		std::lock_guard<std::mutex> lock{ m_Manager.getBuildQueueMutex() };
		if (!m_Manager.getBuildQueue().empty())
			currentChunk = m_Manager.getBuildQueue().back();
	}

	if (currentChunk != nullptr && shouldGen >= genInterval)
	{
		Chunk* adjacentChunks[4];
		m_Manager.getAdjacentChunks(currentChunk->getLocation(), adjacentChunks);

		if (currentChunk->buildMesh(m_Manager, sectionPtr, adjacentChunks));
		{
			std::lock_guard<std::mutex> lock{ m_UploadPendingMutex };
			m_UploadPending.push_back(currentChunk);
		}

		++sectionPtr;

		if (sectionPtr == g_ChunkCap)
		{
			std::lock_guard<std::mutex> lock{ m_Manager.getBuildQueueMutex() };
			m_Manager.getBuildQueue().pop_back();

			currentChunk = nullptr;

			sectionPtr = 0;
		}

		shouldGen = 0;
	}
	++shouldGen;
}

void World::placeQueueBlocks()
{
	std::vector<QueueBlock>& blockQueue{ m_WorldGen.getBlockQueue() };
	if (m_LastBlockQueueSize != blockQueue.size())
	{
		for (int i{}; i < blockQueue.size(); ++i)
		{
			QueueBlock queueBlock{ blockQueue.at(i) };
			Chunk* chunk{ m_Manager.getChunk(queueBlock.loc.worldPos) };

			if (!chunk)
				continue;
			else
			{
				m_Manager.getChunk(queueBlock.loc.worldPos)->getSection(queueBlock.loc.sectionIndex)->setBlock(queueBlock.sectionRelativePos, queueBlock.block.getType(), queueBlock.block.isSurface());
				blockQueue.erase(blockQueue.begin() + i);
				--i;
			}
		}
	}

	m_LastBlockQueueSize = blockQueue.size();
}

void World::rebuildChunks(const Camera& camera)
{
	{
		std::lock_guard<std::mutex> lock{ m_ChunksMutex };

		for (int i{}; i < m_Chunks.size(); ++i)
		{
			m_Chunks[i]->clearMesh();
		}
	}

	{
		std::lock_guard<std::mutex> lock{ m_Manager.getBuildQueueMutex() };
		m_Manager.getBuildQueue().clear();
	}
	
	m_Manager.updateQueues(camera);
	m_ResetBuildVars = true;
}

int World::getChunkIndex(Vector2i chunkPos)
{
	std::lock_guard<std::mutex> lock{ m_ChunksMutex };

	for (int i{}; i < m_Chunks.size(); ++i)
	{
		if (m_Chunks[i]->getLocation() == chunkPos)
			return i;
	}

	return -1;
}

std::mutex& World::getChunksMutex()
{
	return m_ChunksMutex;
}

std::vector<Chunk*>& World::getChunks()
{
	return m_Chunks;
}

std::vector<Chunk*>& World::getUploadPending()
{
	return m_UploadPending;
}

ChunkManager& World::getManager()
{
	return m_Manager;
}

Player& World::getPlayer()
{
	return m_Player;
}

bool World::shouldCloseChunkerThread()
{
	return m_ShouldCloseChunkerThread;
}

void World::setShouldCloseChunkerThread(bool close)
{
	m_ShouldCloseChunkerThread = close;
}