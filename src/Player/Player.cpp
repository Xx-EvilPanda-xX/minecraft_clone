#include "Player.h"
#include "../World/Chunk.h"

Player::Player(const Camera& cam, ChunkManager* manager, float reach) 
	: m_Cam{ cam },
	m_Manager{ manager },
	m_Reach{ reach }
{
}

void Player::breakBlock()
{
	glm::vec3 camFront{ m_Cam.getFront() * 0.1f };
	glm::vec3 currentPos{ m_Cam.getLocation() };
	Vector3i* breakPos{ nullptr };

	while (glm::length((m_Cam.getLocation() - currentPos)) < m_Reach)
	{
		Vector3i blockPos{ static_cast<int>(currentPos.x), static_cast<int>(currentPos.y), static_cast<int>(currentPos.z) };
		if (currentPos.x < 0.0f)
			blockPos.x -= 1;

		if (currentPos.y < 0.0f)
			blockPos.y -= 1;

		if (currentPos.z < 0.0f)
			blockPos.z -= 1;

		if (m_Manager->getWorldBlock(blockPos).getType() != BlockType::Air)
		{
			breakPos = new Vector3i{ blockPos };
			break;
		}

		currentPos += camFront;
	}

	if (breakPos != nullptr)
	{
		Vector3i pos{ *breakPos };

		m_Manager->setWorldBlock(pos, Block{ BlockType::Air });

		Chunk* chunk{ m_Manager->getChunk(pos) };

		chunk->clearMesh();
		chunk->buildMesh(*m_Manager, pos.y / 16);

		Vector3i sectionLocal{ pos.x % 16, pos.y % 16, pos.z % 16 };
		if (pos.x < 0 && sectionLocal.x != 0)
			sectionLocal.x += 16;
		if (pos.y < 0 && sectionLocal.y != 0)
			sectionLocal.y += 16;
		if (pos.z < 0 && sectionLocal.z != 0)
			sectionLocal.z += 16;

		if (sectionLocal.x == 15)
		{
			Chunk* chunkPosX{ m_Manager->getChunk(Vector2i{ chunk->getLocation().x + 1, chunk->getLocation().y }) };

			chunkPosX->clearMesh();
			chunkPosX->buildMesh(*m_Manager, pos.y / 16);
		}

		if (sectionLocal.x == 0)
		{
			Chunk* chunkNegX{ m_Manager->getChunk(Vector2i{ chunk->getLocation().x - 1, chunk->getLocation().y }) };

			chunkNegX->clearMesh();
			chunkNegX->buildMesh(*m_Manager, pos.y / 16);
		}

		if (sectionLocal.z == 15)
		{
			Chunk* chunkPosY{ m_Manager->getChunk(Vector2i{ chunk->getLocation().x, chunk->getLocation().y + 1 }) };

			chunkPosY->clearMesh();
			chunkPosY->buildMesh(*m_Manager, pos.y / 16);
		}

		if (sectionLocal.z == 0)
		{
			Chunk* chunkNegY{ m_Manager->getChunk(Vector2i{ chunk->getLocation().x, chunk->getLocation().y - 1 }) };

			chunkNegY->clearMesh();
			chunkNegY->buildMesh(*m_Manager, pos.y / 16);
		}

		delete breakPos;
	}
}

void placeBlock()
{

}

float Player::getReach() const
{
	return m_Reach;
}

void Player::setReach(float reach)
{
	m_Reach = reach;
}

void Player::setManager(ChunkManager* manager)
{
	m_Manager = manager;
}
