#include "Player.h"
#include "../World/Chunk.h"
#include <glm/vec3.hpp>
#include "../Application.h"
#include "../Constants.h"

Player::Player(Camera& cam, ChunkManager* manager, float reach) 
	: m_Cam{ cam },
	m_Manager{ manager },
	m_Reach{ reach }
{
}

void Player::move()
{
	calculateVelocity(); 

	aabb.min(glm::vec3{ m_Cam.getLocation().x - 0.45f, m_Cam.getLocation().y - 1.45f, m_Cam.getLocation().z - 0.45f });
	aabb.max(glm::vec3{ m_Cam.getLocation().x + 0.45f, m_Cam.getLocation().y + 0.45f, m_Cam.getLocation().z + 0.45f });

	Vector3i playerPos{ static_cast<int>(m_Cam.getLocation().x), static_cast<int>(m_Cam.getLocation().y), static_cast<int>(m_Cam.getLocation().z) };

	if (m_Cam.getLocation().x < 0.0f)
		playerPos.x -= 1;
	if (m_Cam.getLocation().y < 0.0f)
		playerPos.y -= 1;
	if (m_Cam.getLocation().z < 0.0f)
		playerPos.z -= 1;
	
	bool collision{ false };
	Vector3i collisionPos{};
	Block collisionBlock{};

	for (int x{ playerPos.x - 2 }; x < playerPos.x + 2; ++x)
	{
		for (int y{ playerPos.y - 2 }; y < playerPos.y + 2; ++y)
		{
			for (int z{ playerPos.z - 2 }; z < playerPos.z + 2; ++z)
			{
				Block block{ m_Manager->getWorldBlock(Vector3i{ x, y, z }) };
				if (block.getBounds().intersects(aabb) && block.getType() != BlockType::Air)
				{
					std::cout << "collision with block at: " << x << ", " << y << ", " << z << "\n";
					collision = true;
					collisionPos = { x, y, z };
					collisionBlock = block;
					break;
				}
			}
		}
	}

	if (collision)
	{
		m_Cam.setLocation(m_LastValidLoc);
		m_Velocity *= 0.0f;

		/*
		* get vector from position right before collision to the position right after 
		* and do a binary search along that vector until the vector is right outside of the 
		* colliding block. Then take that vector and the center of the colliding block and
		* follow it out to the near adjacent block that passes through that vector, similar
		* to block placing. Then find diff between those two block positions and theres your
		* velocity component to mitigate.
		*/
	}
	else
	{
		m_LastValidLoc = m_Cam.getLocation();
	}

	m_Cam.handleKeyboard(m_Velocity, Application::m_Dt);
}

void Player::calculateVelocity()
{
	if (m_Velocity.x < 0.0f)
	{
		m_Velocity.x += constants::playerDrift * Application::m_Dt;
		if (m_Velocity.x > 0.0f)
			m_Velocity.x = 0.0f;
	}
	else
	{
		m_Velocity.x -= constants::playerDrift * Application::m_Dt;
		if (m_Velocity.x < 0.0f)
			m_Velocity.x = 0.0f;
	}

	if (m_Velocity.y < 0.0f)
	{
		m_Velocity.y += constants::playerDrift * Application::m_Dt;
		if (m_Velocity.y > 0.0f)
			m_Velocity.y = 0.0f;
	}	
	else
	{
		m_Velocity.y -= constants::playerDrift * Application::m_Dt;
		if (m_Velocity.y < 0.0f)
			m_Velocity.y = 0.0f;
	}

	if (m_Velocity.z < 0.0f)
	{
		m_Velocity.z += constants::playerDrift * Application::m_Dt;
		if (m_Velocity.z > 0.0f)
			m_Velocity.z = 0.0f;
	}	
	else
	{
		m_Velocity.z -= constants::playerDrift * Application::m_Dt;
		if (m_Velocity.z < 0.0f)
			m_Velocity.z = 0.0f;
	}

	if ((m_Velocity.x < 0.01f && m_Velocity.x > 0.0f) || (m_Velocity.x > -0.01f && m_Velocity.x < 0.0f))
		m_Velocity.x = 0.0f;
	if ((m_Velocity.y < 0.01f && m_Velocity.y > 0.0f) || (m_Velocity.y > -0.01f && m_Velocity.y < 0.0f))
		m_Velocity.y = 0.0f;
	if ((m_Velocity.z < 0.01f && m_Velocity.z > 0.0f) || (m_Velocity.z > -0.01f && m_Velocity.z < 0.0f))
		m_Velocity.z = 0.0f;
}

void Player::breakBlock()
{
	Vector3i* breakPos{ breakIntersect() };

	if (breakPos != nullptr)
	{
		Vector3i pos{ *breakPos };

		if (!m_Manager->chunkExsists(pos))
			return;

		m_Manager->setWorldBlock(pos, BlockType::Air );

		updateMeshes(pos);

		delete breakPos;
	}
}

void Player::placeBlock(BlockType type)
{
	glm::vec3 camFront{ m_Cam.getFront() * 0.01f };
	glm::vec3* intersect{ placeIntersect() };

	if (intersect != nullptr)
	{
		glm::vec3 pos{ *intersect };
		Vector3i blockPos{ static_cast<int>(intersect->x < 0.0f ? intersect->x - 1.0f : intersect->x), static_cast<int>(intersect->y < 0.0f ? intersect->y - 1.0f : intersect->y), static_cast<int>(intersect->z < 0.0f ? intersect->z - 1.0f : intersect->z) };
		Vector3i placePos{ static_cast<int>(intersect->x < 0.0f ? pos.x - 1.0f : pos.x), static_cast<int>(intersect->y < 0.0f ? pos.y - 1.0f : pos.y), static_cast<int>(intersect->z < 0.0f ? pos.z- 1.0f : pos.z) };
		while (placePos == blockPos)
		{
			pos -= camFront;
			placePos.x = static_cast<int>(pos.x < 0.0f ? pos.x - 1.0f : pos.x);
			placePos.y = static_cast<int>(pos.y < 0.0f ? pos.y - 1.0f : pos.y);
			placePos.z = static_cast<int>(pos.z < 0.0f ? pos.z - 1.0f : pos.z);
		}

		if (pos.y < 0.0f)
			return;

		if (!m_Manager->chunkExsists(placePos))
			return;

		m_Manager->setWorldBlock(placePos, type);

		updateMeshes(placePos);

		delete intersect;
	}
}

void Player::updateMeshes(Vector3i editPos)
{
	Chunk* chunk{ m_Manager->getChunk(editPos) };

	chunk->clearMesh();
	for (int i{}; i < 16; ++i)
	{
		chunk->buildMesh(*m_Manager, i);
	}

	Vector3i local{ sectionLocal(editPos) };

	if (local.x == 15)
	{
		Chunk* chunkPosX{ m_Manager->getChunk(Vector2i{ chunk->getLocation().x + 1, chunk->getLocation().y }) };

		chunkPosX->clearMesh();
		for (int i{}; i < 16; ++i)
		{
			chunkPosX->buildMesh(*m_Manager, i);
		}
	}

	if (local.x == 0)
	{
		Chunk* chunkNegX{ m_Manager->getChunk(Vector2i{ chunk->getLocation().x - 1, chunk->getLocation().y }) };

		chunkNegX->clearMesh();
		for (int i{}; i < 16; ++i)
		{
			chunkNegX->buildMesh(*m_Manager, i);
		}
	}

	if (local.z == 15)
	{
		Chunk* chunkPosY{ m_Manager->getChunk(Vector2i{ chunk->getLocation().x, chunk->getLocation().y + 1 }) };

		chunkPosY->clearMesh();
		for (int i{}; i < 16; ++i)
		{
			chunkPosY->buildMesh(*m_Manager, i);
		}
	}

	if (local.z == 0)
	{
		Chunk* chunkNegY{ m_Manager->getChunk(Vector2i{ chunk->getLocation().x, chunk->getLocation().y - 1 }) };

		chunkNegY->clearMesh();
		for (int i{}; i < 16; ++i)
		{
			chunkNegY->buildMesh(*m_Manager, i);
		}
	}
}

Vector3i Player::sectionLocal(Vector3i worldPos)
{
	Vector3i sectionLocal{ worldPos.x % 16, worldPos.y % 16, worldPos.z % 16 };
	if (worldPos.x < 0 && sectionLocal.x != 0)
		sectionLocal.x += 16;
	if (worldPos.y < 0 && sectionLocal.y != 0)
		sectionLocal.y += 16;
	if (worldPos.z < 0 && sectionLocal.z != 0)
		sectionLocal.z += 16;

	return sectionLocal;
}

Vector3i* Player::breakIntersect()
{
	glm::vec3 camFront{ m_Cam.getFront() * 0.01f };
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

	return breakPos;
}

glm::vec3* Player::placeIntersect()
{
	glm::vec3 camFront{ m_Cam.getFront() * 0.01f };
	glm::vec3 currentPos{ m_Cam.getLocation() };
	glm::vec3* placePos{ nullptr };

	while (glm::length((m_Cam.getLocation() - currentPos)) < m_Reach)
	{
		Vector3i blockPos{ static_cast<int>(currentPos.x), static_cast<int>(currentPos.y), static_cast<int>(currentPos.z) };
		if (currentPos.x < 0.0f)
			blockPos.x -= 1.0f;

		if (currentPos.y < 0.0f)
			blockPos.y -= 1.0f;

		if (currentPos.z < 0.0f)
			blockPos.z -= 1.0f;

		if (m_Manager->getWorldBlock(blockPos).getType() != BlockType::Air)
		{
			placePos = new glm::vec3{ currentPos };
			break;
		}

		currentPos += camFront;
	}

	return placePos;
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

glm::vec3& Player::getVelocity()
{
	return m_Velocity;
}

bool Player::isSprinting()
{
	return m_Sprinting;
}

void Player::setSprinting(bool sprinting)
{
	m_Sprinting = sprinting;
}
