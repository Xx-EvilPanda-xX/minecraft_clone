#include "World.h"
#include <iostream>
#include <cmath>
#include <GLFW/glfw3.h>
#include "Chunk.h"
#include "../Math/Vector2i.h"
#include "../Render/Shader.h"
#include "ChunkManager.h"
#include "../Constants.h"

//#define DEBUG
constexpr int chunkBuildsPerFrame{ constants::renderDistance / 2 };

World::World(Shader shader, Player player, ChunkManager& manager)
	: m_Shader{ shader },
	m_Manager{ manager },
	m_Player{ player }
{
}

void World::worldUpdate(const Camera& camera, bool deletePass)
{
	Vector3i playerPos{ getPlayerBlockPos(camera) };

	if (m_Manager.chunkExsists(playerPos))
		m_Player.move();

	m_Manager.updateQueues(camera);

	for (int i{}; i < chunkBuildsPerFrame; ++i)
	{
		genPass();
		buildPass();
	}

	if (deletePass)
		destroyPass(Vector2i{ playerPos.x, playerPos.z });
}

void World::worldRender(const Camera& camera)
{
	Vector3i playerPos{ getPlayerBlockPos(camera) };
	m_Shader.bind();
	m_Shader.setBool("playerUnderWater", m_Manager.getWorldBlock(playerPos).getType() == BlockType::Water);
	m_Shader.unbind();

	for (std::pair<Vector2i, Chunk*> p : m_Chunks)
	{
		Chunk* chunk{ p.second };
		if (chunk->isBuilt())
			Renderer::drawMesh(camera, chunk->getMesh());
	}
}

void World::genPass()
{
	if (!m_Manager.getGenQueue().empty())
	{
		Vector2i chunkPos{ m_Manager.getGenQueue().back() };
		m_Manager.getGenQueue().pop_back();

#ifdef DEBUG
		if (chunkPos.x > 4 || chunkPos.x < -4 || chunkPos.y > 4 || chunkPos.y < -4)
			return;
#endif // DEBUG


		Chunk* chunk{ new Chunk(chunkPos, m_Shader) };

		int** heightMap{ m_WorldGen.getHeightMap(chunk) };
		for (int k{}; k < g_ChunkCap; ++k)
		{
			chunk->addSection(m_WorldGen.genSection(heightMap, k));
		}

		for (int i{}; i < 16; ++i)
		{
			delete[] heightMap[i];
		}

		delete[] heightMap;

		m_Chunks.insert({ chunkPos, chunk });
	}
}

void World::destroyPass(Vector2i playerPos)
{
	playerPos.x /= 16;
	playerPos.y /= 16;

	for (std::pair<Vector2i, Chunk*> p : m_Chunks)
	{
		Vector2i chunkLoc{ p.first };
		Chunk* chunk{ p.second };

		//+ 1 becuase otherwise chunks that had just been generated got deleted
		if (std::abs(chunkLoc.x - playerPos.x) > constants::renderDistance + 1 || std::abs(chunkLoc.y - playerPos.y) > constants::renderDistance + 1)
		{
			int index;
			if (m_Manager.isInBuildQueue(chunk, index))
				m_Manager.getBuildQueue().erase(m_Manager.getBuildQueue().begin() + index);

			Vector2i chunkLoc{ chunk->getLocation() };

			if (m_Manager.isInBuildQueue(m_Manager.getChunk(Vector2i{ chunkLoc.x + 1, chunkLoc.y }), index))
				m_Manager.getBuildQueue().erase(m_Manager.getBuildQueue().begin() + index);

			if (m_Manager.isInBuildQueue(m_Manager.getChunk(Vector2i{ chunkLoc.x, chunkLoc.y + 1 }), index))
				m_Manager.getBuildQueue().erase(m_Manager.getBuildQueue().begin() + index);

			if (m_Manager.isInBuildQueue(m_Manager.getChunk(Vector2i{ chunkLoc.x - 1, chunkLoc.y }), index))
				m_Manager.getBuildQueue().erase(m_Manager.getBuildQueue().begin() + index);

			if (m_Manager.isInBuildQueue(m_Manager.getChunk(Vector2i{ chunkLoc.x, chunkLoc.y - 1 }), index))
				m_Manager.getBuildQueue().erase(m_Manager.getBuildQueue().begin() + index);

			delete chunk;
			m_Chunks.erase(chunkLoc);
		}
	}
}

void World::buildPass()
{
	static int sectionPtr{};
	static int shouldGen{};

	Chunk* currentChunk{ nullptr };

	if (!m_Manager.getBuildQueue().empty())
		currentChunk = m_Manager.getBuildQueue().back();

	if (currentChunk != nullptr && shouldGen >= genInterval)
	{
		Chunk* adjacentChunks[4];
		m_Manager.getAdjacentChunks(currentChunk->getLocation(), adjacentChunks);
		currentChunk->buildMesh(m_Manager, sectionPtr, adjacentChunks);
		++sectionPtr;

		if (sectionPtr == g_ChunkCap)
		{
			m_Manager.getBuildQueue().pop_back();

			currentChunk = nullptr;

			sectionPtr = 0;
		}

		shouldGen = 0;
	}
	++shouldGen;
}

void World::reloadChunks(const Camera& camera)
{
	for (std::pair<Vector2i, Chunk*> p : m_Chunks)
	{
		delete p.second;
		m_Chunks[p.first] = nullptr;
	}

	m_Chunks.clear();
	m_Manager.clearQueues();
	m_Manager.updateQueues(camera);
}

Vector3i World::getPlayerBlockPos(const Camera& camera)
{
	Vector3i playerPos{ static_cast<int>(camera.getLocation().x), static_cast<int>(camera.getLocation().y), static_cast<int>(camera.getLocation().z) };

	if (camera.getLocation().x < 0.0)
		--playerPos.x;
	if (camera.getLocation().y < 0.0)
		--playerPos.y;
	if (camera.getLocation().z < 0.0)
		--playerPos.z;

	return playerPos;
}

std::unordered_map<Vector2i, Chunk*>& World::getChunks()
{
	return m_Chunks;
}

ChunkManager& World::getManager()
{
	return m_Manager;
}

Player& World::getPlayer()
{
	return m_Player;
}