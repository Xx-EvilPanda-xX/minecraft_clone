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
	m_Velocity = glm::vec3{};
	m_LastValidLoc = glm::vec3{};
}

void Player::move()
{
	m_Cam.handleKeyboard(m_Velocity, Application::m_Dt);
	calculateVelocity();

	Vector3i collsionPos{};
	AABB blockAABB{};
	bool update{ true };

	m_Aabb = createPlayerAABB(m_Cam.getLocation());

	while (testCollide(m_Cam.getLocation(), m_Aabb, collsionPos, blockAABB))
	{
		
		glm::vec3 blockCenter{ collsionPos.x + 0.5f, collsionPos.y + 0.5f, collsionPos.z + 0.5f };
		glm::vec3 direction{ glm::normalize(m_LastValidLoc - blockCenter) * 0.001f };

		glm::vec3 center{ blockCenter };
		Vector3i block{ static_cast<int>(center.x < 0.0f ? center.x - 1.0f : center.x), static_cast<int>(center.y < 0.0f ? center.y - 1.0f : center.y), static_cast<int>(center.z < 0.0f ? center.z - 1.0f : center.z) };
		while (block == collsionPos)
		{
			center += direction;
			block.x = static_cast<int>(center.x < 0.0f ? center.x - 1.0f : center.x);
			block.y = static_cast<int>(center.y < 0.0f ? center.y - 1.0f : center.y);
			block.z = static_cast<int>(center.z < 0.0f ? center.z - 1.0f : center.z);
		}

		glm::vec3 camPos{ m_Cam.getLocation() };
		int intersects{};

		bool collideX{};
		bool collideY{};
		bool collideZ{};

		if (block.x != collsionPos.x)
		{
			collideX = true;
			++intersects;
		}

		if (block.y != collsionPos.y)
		{
			collideY = true;
			++intersects;
		}

		if (block.z != collsionPos.z)
		{
			collideZ = true;
			++intersects;
		}

		if (intersects > 1)
		{
			if (m_LastCollideX)
				m_Cam.setLocation(glm::vec3{ m_LastValidLoc.x, camPos.y, camPos.z });

			if (m_LastCollideY)
				m_Cam.setLocation(glm::vec3{ camPos.x, m_LastValidLoc.y, camPos.z });

			if (m_LastCollideZ)
				m_Cam.setLocation(glm::vec3{ camPos.x, camPos.y, m_LastValidLoc.z });
		}
		else
		{
			if (collideX)
				m_Cam.setLocation(glm::vec3{ m_LastValidLoc.x, camPos.y, camPos.z });

			if (collideY)
				m_Cam.setLocation(glm::vec3{ camPos.x, m_LastValidLoc.y, camPos.z });

			if (collideZ)
				m_Cam.setLocation(glm::vec3{ camPos.x, camPos.y, m_LastValidLoc.z });

			m_LastCollideX = collideX;
			m_LastCollideY = collideY;
			m_LastCollideZ = collideZ;
		}

		m_Aabb = createPlayerAABB(m_Cam.getLocation());
	}

	m_LastValidLoc = m_Cam.getLocation();
	m_LastValidAABB = createPlayerAABB(m_LastValidLoc);
}

AABB Player::createPlayerAABB(glm::vec3 playerPos)
{
	AABB aabb{};
	aabb.min(glm::vec3{ playerPos.x - 0.35f, playerPos.y - 0.35f, playerPos.z - 0.35f });
	aabb.max(glm::vec3{ playerPos.x + 0.35f, playerPos.y + 0.35f, playerPos.z + 0.35f });
	return aabb;
}

bool Player::testCollide(glm::vec3 playerPos, AABB& playerAABB, Vector3i& o_Pos, AABB& o_Aabb)
{
	Vector3i playerBlock{ static_cast<int>(m_Cam.getLocation().x), static_cast<int>(m_Cam.getLocation().y), static_cast<int>(m_Cam.getLocation().z) };

	if (playerPos.x < 0.0f)
		playerBlock.x -= 1;
	if (playerPos.y < 0.0f)
		playerBlock.y -= 1;
	if (playerPos.z < 0.0f)
		playerBlock.z -= 1;

	bool collision{ false };
	float closestCollision{ -1.0f };
	Vector3i collsionPos{};

	for (int x{ playerBlock.x - 2 }; x < playerBlock.x + 2; ++x)
	{
		for (int y{ playerBlock.y - 2 }; y < playerBlock.y + 2; ++y)
		{
			for (int z{ playerBlock.z - 2 }; z < playerBlock.z + 2; ++z)
			{
				AABB blockAABB{ glm::vec3{ x, y, z }, glm::vec3{ x + 1, y + 1, z + 1 } };
				Block block{ m_Manager->getWorldBlock(Vector3i{ x, y, z }) };
				if (blockAABB.intersects(playerAABB) && block.getType() != BlockType::Air)
				{
					glm::vec3 blockCenter{ x + 0.5f, y + 0.5f, z + 0.5f };
					glm::vec3 playerToCenter{ blockCenter - playerPos };
					float distance{ glm::length(playerToCenter) };

					collision = true;

					if (distance < closestCollision || closestCollision == -1.0f)
					{
						closestCollision = distance;
						collsionPos = { x, y, z };
						o_Aabb = blockAABB;
					}
				}
			}
		}
	}

	o_Pos = collsionPos;
	return collision;
}

bool Player::testCollide(glm::vec3 playerPos, AABB& playerAABB)
{
	Vector3i playerBlock{ static_cast<int>(m_Cam.getLocation().x), static_cast<int>(m_Cam.getLocation().y), static_cast<int>(m_Cam.getLocation().z) };

	if (playerPos.x < 0.0f)
		playerBlock.x -= 1;
	if (playerPos.y < 0.0f)
		playerBlock.y -= 1;
	if (playerPos.z < 0.0f)
		playerBlock.z -= 1;

	bool collision{ false };
	float closestCollision{ -1.0f };
	Vector3i collsionPos{};

	for (int x{ playerBlock.x - 2 }; x < playerBlock.x + 2; ++x)
	{
		for (int y{ playerBlock.y - 2 }; y < playerBlock.y + 2; ++y)
		{
			for (int z{ playerBlock.z - 2 }; z < playerBlock.z + 2; ++z)
			{
				AABB blockAABB{ glm::vec3{ x, y, z }, glm::vec3{ x + 1, y + 1, z + 1 } };
				Block block{ m_Manager->getWorldBlock(Vector3i{ x, y, z }) };
				if (blockAABB.intersects(playerAABB) && block.getType() != BlockType::Air)
				{
					glm::vec3 blockCenter{ x + 0.5f, y + 0.5f, z + 0.5f };
					glm::vec3 playerToCenter{ blockCenter - playerPos };
					float distance{ glm::length(playerToCenter) };

					collision = true;

					if (distance < closestCollision || closestCollision == -1.0f)
					{
						closestCollision = distance;
						collsionPos = { x, y, z };
					}
				}
			}
		}
	}

	return collision;
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

		AABB blockAABB{ glm::vec3{ placePos.x, placePos.y, placePos.z }, glm::vec3{ placePos.x + 1, placePos.y + 1, placePos.z + 1 } };
		if (m_Aabb.intersects(blockAABB))
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
