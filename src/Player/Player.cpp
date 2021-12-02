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

	m_Rand = std::mt19937{ static_cast<std::mt19937::result_type>(std::time(nullptr)) };
	m_Die = std::uniform_int_distribution<>{ 0, 2 };
}

void Player::move()
{
	calculateVelocity();
	m_Cam.handleKeyboard(m_Velocity, Application::m_Dt);

	//gravity
	if (!m_Flying && !m_Grounded)
		m_Velocity.y -= Application::m_Dt * constants::gravity;

	if (m_Velocity.z == 0.0f)
		int i{};

	Vector3i collsionPos{};
	bool onGround{ false };

	m_Aabb = createPlayerAABB(m_Cam.getLocation());

	float playerHeight{ m_Aabb.max().y - m_Aabb.min().y };

	glm::vec3 lowerPlayerHalf{ m_Cam.getLocation().x, m_Aabb.min().y + constants::playerSize, m_Cam.getLocation().z };
	glm::vec3 upperPlayerHalf{ m_Cam.getLocation().x, m_Aabb.max().y - constants::playerSize, m_Cam.getLocation().z };

	CollsionType collisionType;

	while (testCollide(lowerPlayerHalf, upperPlayerHalf, m_Aabb, collsionPos, collisionType))
	{
		glm::vec3 blockCenter{ collsionPos.x + 0.5f, collsionPos.y + 0.5f, collsionPos.z + 0.5f };
		glm::vec3 lastValidLoc;

		switch (collisionType)
		{
		case CollsionType::PlayerUpperHalf:
			lastValidLoc = { m_LastValidLoc.x, m_LastValidAABB.max().y - constants::playerSize, m_LastValidLoc.z };
			break;
		case CollsionType::PlayerLowerHalf:
			lastValidLoc = { m_LastValidLoc.x, m_LastValidAABB.min().y + constants::playerSize, m_LastValidLoc.z };
			break;
		}

		glm::vec3 direction{ glm::normalize(lastValidLoc - blockCenter) * 0.01f };
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

		if (intersects != 1)
		{
			if (!m_LastCollideX && !m_LastCollideY && !m_LastCollideZ)
				m_Cam.setLocation(m_LastValidLoc);

			if ((m_LastCollideX && m_LastCollideY) || (m_LastCollideX && m_LastCollideZ) || (m_LastCollideY && m_LastCollideZ) || (m_LastCollideX && m_LastCollideY && m_LastCollideZ))
			{
				int rand = m_Die(m_Rand);

				switch (rand)
				{
				case 0:
					if (m_LastCollideX)
					{
						m_LastCollideY = false;
						m_LastCollideZ = false;
					}
					break;

				case 1:
					if (m_LastCollideY)
					{
						m_LastCollideX = false;
						m_LastCollideZ = false;
					}
					break;
				
				case 2:
					if (m_LastCollideZ)
					{
						m_LastCollideX = false;
						m_LastCollideY = false;
					}
					break;
				}
			}

			if (m_LastCollideX)
				m_Cam.setLocation(glm::vec3{ m_LastValidLoc.x, camPos.y, camPos.z });

			if (m_LastCollideY)
			{
				m_Cam.setLocation(glm::vec3{ camPos.x, m_LastValidLoc.y, camPos.z });
				m_Velocity.y = 0.0f;
				onGround = true;
			}

			if (m_LastCollideZ)
				m_Cam.setLocation(glm::vec3{ camPos.x, camPos.y, m_LastValidLoc.z });
		}
		else
		{
			if (collideX)
			{
				m_Cam.setLocation(glm::vec3{ m_LastValidLoc.x, camPos.y, camPos.z });
			}

			if (collideY)
			{
				m_Cam.setLocation(glm::vec3{ camPos.x, m_LastValidLoc.y, camPos.z });
				m_Velocity.y = 0.0f;
				onGround = true;
			}

			if (collideZ)
			{
				m_Cam.setLocation(glm::vec3{ camPos.x, camPos.y, m_LastValidLoc.z });
			}
		}

		m_LastCollideX = collideX;
		m_LastCollideY = collideY;
		m_LastCollideZ = collideZ;

		m_Aabb = createPlayerAABB(m_Cam.getLocation());

		playerHeight = m_Aabb.max().y - m_Aabb.min().y;

		lowerPlayerHalf = { m_Cam.getLocation().x, m_Aabb.min().y + constants::playerSize, m_Cam.getLocation().z };
		upperPlayerHalf = { m_Cam.getLocation().x, m_Aabb.max().y - constants::playerSize, m_Cam.getLocation().z };
	}

	m_Grounded = onGround;

	m_LastValidLoc = m_Cam.getLocation();
	m_LastValidAABB = createPlayerAABB(m_LastValidLoc);

	++m_Moves;
}

AABB Player::createPlayerAABB(glm::vec3 playerPos)
{
	AABB aabb{};
	aabb.min(glm::vec3{ playerPos.x - constants::playerSize, playerPos.y - (1.0f + constants::playerSize), playerPos.z - constants::playerSize });
	aabb.max(glm::vec3{ playerPos.x + constants::playerSize, playerPos.y + constants::playerSize, playerPos.z + constants::playerSize });
	return aabb;
}

bool Player::testCollide(glm::vec3 playerLowerHalf, glm::vec3 playerUpperHalf, AABB& playerAABB, Vector3i& o_Pos, CollsionType& o_CollisionType)
{
	Vector3i lowerBlock{ static_cast<int>(playerLowerHalf.x), static_cast<int>(playerLowerHalf.y), static_cast<int>(playerLowerHalf.z) };
	Vector3i upperBlock{ static_cast<int>(playerUpperHalf.x), static_cast<int>(playerUpperHalf.y), static_cast<int>(playerUpperHalf.z) };

	if (playerLowerHalf.x < 0.0f)
		lowerBlock.x -= 1;
	if (playerLowerHalf.y < 0.0f)
		lowerBlock.y -= 1;
	if (playerLowerHalf.z < 0.0f)
		lowerBlock.z -= 1;

	if (playerUpperHalf.x < 0.0f)
		upperBlock.x -= 1;
	if (playerUpperHalf.y < 0.0f)
		upperBlock.y -= 1;
	if (playerUpperHalf.z < 0.0f)
		upperBlock.z -= 1;

	bool collision{ false };

	float lowerClosestCollision{ -1.0f };
	Vector3i lowerCollsionPos{};
	AABB lowerAABB{};

	float upperClosestCollision{ -1.0f };
	Vector3i upperCollsionPos{};
	AABB upperAABB{};

	for (int x{ lowerBlock.x - 2 }; x < lowerBlock.x + 2; ++x)
	{
		for (int y{ lowerBlock.y - 2 }; y < lowerBlock.y + 2; ++y)
		{
			for (int z{ lowerBlock.z - 2 }; z < lowerBlock.z + 2; ++z)
			{
				AABB blockAABB{ glm::vec3{ x, y, z }, glm::vec3{ x + 1, y + 1, z + 1 } };
				Block block{ m_Manager->getWorldBlock(Vector3i{ x, y, z }) };
				if (blockAABB.intersects(playerAABB) && block.getType() != BlockType::Air)
				{
					glm::vec3 blockCenter{ x + 0.5f, y + 0.5f, z + 0.5f };
					glm::vec3 playerToCenter{ blockCenter - playerLowerHalf };
					float distance{ glm::length(playerToCenter) };

					collision = true;

					if (distance < lowerClosestCollision || lowerClosestCollision == -1.0f)
					{
						lowerClosestCollision = distance;
						lowerCollsionPos = { x, y, z };
						lowerAABB = blockAABB;
					}
				}
			}
		}
	}

	for (int x{ upperBlock.x - 2 }; x < upperBlock.x + 2; ++x)
	{
		for (int y{ upperBlock.y - 2 }; y < upperBlock.y + 2; ++y)
		{
			for (int z{ upperBlock.z - 2 }; z < upperBlock.z + 2; ++z)
			{
				AABB blockAABB{ glm::vec3{ x, y, z }, glm::vec3{ x + 1, y + 1, z + 1 } };
				Block block{ m_Manager->getWorldBlock(Vector3i{ x, y, z }) };
				if (blockAABB.intersects(playerAABB) && block.getType() != BlockType::Air)
				{
					glm::vec3 blockCenter{ x + 0.5f, y + 0.5f, z + 0.5f };
					glm::vec3 playerToCenter{ blockCenter - playerUpperHalf };
					float distance{ glm::length(playerToCenter) };

					collision = true;

					if (distance < upperClosestCollision || upperClosestCollision == -1.0f)
					{
						upperClosestCollision = distance;
						upperCollsionPos = { x, y, z };
						upperAABB = blockAABB;
					}
				}
			}
		}
	}

	if (lowerClosestCollision != -1.0f)
	{
		if (lowerClosestCollision <= upperClosestCollision)
		{
			o_Pos = lowerCollsionPos;
			o_CollisionType = CollsionType::PlayerLowerHalf;
		}
		else if (upperClosestCollision <= lowerClosestCollision)
		{
			o_Pos = upperCollsionPos;
			o_CollisionType = CollsionType::PlayerUpperHalf;
		}
	}

	else if (upperClosestCollision != -1.0f)
	{
		if (upperClosestCollision <= lowerClosestCollision)
		{
			o_Pos = upperCollsionPos;
			o_CollisionType = CollsionType::PlayerUpperHalf;
		}
		else if (lowerClosestCollision <= upperClosestCollision)
		{
			o_Pos = upperCollsionPos;
			o_CollisionType = CollsionType::PlayerUpperHalf;
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

bool Player::isGrounded()
{
	return m_Grounded;
}

bool Player::isFlying()
{
	return m_Flying;
}

void Player::setFlying(bool flying)
{
	m_Flying = flying;
}

void Player::setSprinting(bool sprinting)
{
	m_Sprinting = sprinting;
}
