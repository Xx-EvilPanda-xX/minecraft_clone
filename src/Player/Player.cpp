#include "Player.h"
#include "../World/Chunk.h"
#include <glm/vec3.hpp>
#include "../Application.h"
#include "../Constants.h"
#include <stack>

Player::Player(Camera & cam, ChunkManager & manager, float reach)
	: m_Cam{ cam },
	m_Manager{ manager },
	m_Reach{ reach }
{
	m_Velocity = glm::vec3{};
	m_LastValidLoc = glm::vec3{};
}

void Player::move()
{
	calculateVelocity();

	//gravity
	if (!m_Flying && !m_Grounded)
		m_Velocity.y -= Application::s_Dt * constants::gravity;

	m_Cam.handleKeyboard(m_Velocity, Application::s_Dt);

	Vector3i collsionPos{};
	bool onGround{ false };

	m_Aabb = createPlayerAABB(m_Cam.getLocation());

	glm::vec3 lowerPlayerHalf{ m_Cam.getLocation().x, m_Aabb.min().y + constants::playerSize, m_Cam.getLocation().z };
	glm::vec3 upperPlayerHalf{ m_Cam.getLocation().x, m_Aabb.max().y - constants::playerSize, m_Cam.getLocation().z };

	CollsionType collisionType{};

	//3 iters, one per axis
	for (int i{}; testCollide(lowerPlayerHalf, upperPlayerHalf, m_Aabb, collsionPos, collisionType) && i < 3; ++i)
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

		glm::vec3 direction{ glm::normalize(lastValidLoc - blockCenter) * vecPrecision };
		glm::vec3 center{ blockCenter };
		Vector3i block{ static_cast<int>(center.x < 0.0f ? center.x - 1.0f : center.x), static_cast<int>(center.y < 0.0f ? center.y - 1.0f : center.y), static_cast<int>(center.z < 0.0f ? center.z - 1.0f : center.z) };
		while (block == collsionPos)
		{
			center += direction;
			block.x = static_cast<int>(center.x < 0.0f ? center.x - 1.0f : center.x);
			block.y = static_cast<int>(center.y < 0.0f ? center.y - 1.0f : center.y);
			block.z = static_cast<int>(center.z < 0.0f ? center.z - 1.0f : center.z);
		}

		int intersects{};

		bool collideX{ block.x != collsionPos.x };
		bool collideY{ block.y != collsionPos.y };
		bool collideZ{ block.z != collsionPos.z };

		if (collideX)
			++intersects;

		if (collideY)
			++intersects;

		if (collideZ)
			++intersects;

		if (intersects != 1)
		{
			collideX = false;
			collideY = false;
			collideZ = false;

			if (!m_LastMovedX && m_LastMovedY && m_LastMovedZ)
				collideX = true;

			if (m_LastMovedX && !m_LastMovedY && m_LastMovedZ)
			{
				if (blockCenter.y > m_Aabb.min().y && blockCenter.y < m_Aabb.max().y)
				{
					m_LastMovedX = !m_LastMovedX;
					m_LastMovedY = !m_LastMovedY;
					m_LastMovedZ = !m_LastMovedZ;
				}
				else
					collideY = true;
			}	

			if (m_LastMovedX && m_LastMovedY && !m_LastMovedZ)
				collideZ = true;

			if (!m_LastMovedX && !m_LastMovedY && m_LastMovedZ)
				collideX = true;

			if (m_LastMovedX && !m_LastMovedY && !m_LastMovedZ)
				collideZ = true;

			if (!m_LastMovedX && m_LastMovedY && !m_LastMovedZ)
				collideX = true;

			if (m_LastMovedX && m_LastMovedY && m_LastMovedZ)
				collideX = true;
		}

		glm::vec3 camPos{ m_Cam.getLocation() };
		const float targetDistance{ 0.5f + constants::playerSize + 0.0001f };

		if (collideX)
		{
			float centerToPlayerDistance{ std::abs(blockCenter.x - lastValidLoc.x) };
			float change{ centerToPlayerDistance - targetDistance };

			if (change > 0.1f)
				std::cout << "pum\n";

			m_Cam.setLocation(glm::vec3{ m_LastValidLoc.x + (blockCenter.x < lastValidLoc.x ? -change : change), camPos.y, camPos.z });
		}

		if (collideY)
		{
			float centerToPlayerDistance{ std::abs(blockCenter.y - lastValidLoc.y) };
			float change{ centerToPlayerDistance - targetDistance };

			if (change > 0.1f)
				std::cout << "pum\n";

			m_Cam.setLocation(glm::vec3{ camPos.x, m_LastValidLoc.y + (blockCenter.y < lastValidLoc.y ? -change : change), camPos.z });
			m_Velocity.y = 0.0f;

			if (collisionType == CollsionType::PlayerLowerHalf)
				onGround = true;
		}

		if (collideZ)
		{
			float centerToPlayerDistance{ std::abs(blockCenter.z - lastValidLoc.z) };
			float change{ centerToPlayerDistance - targetDistance };

			if (change > 0.1f)
				std::cout << "pum\n";

			m_Cam.setLocation(glm::vec3{ camPos.x, camPos.y, m_LastValidLoc.z + (blockCenter.z < lastValidLoc.z ? -change : change) });
		}

		m_Aabb = createPlayerAABB(m_Cam.getLocation());

		lowerPlayerHalf = { m_Cam.getLocation().x, m_Aabb.min().y + constants::playerSize, m_Cam.getLocation().z };
		upperPlayerHalf = { m_Cam.getLocation().x, m_Aabb.max().y - constants::playerSize, m_Cam.getLocation().z };
	}

	m_Grounded = onGround;

	m_LastMovedX = m_LastValidLoc.x != m_Cam.getLocation().x;
	m_LastMovedY = m_LastValidLoc.y != m_Cam.getLocation().y;
	m_LastMovedZ = m_LastValidLoc.z != m_Cam.getLocation().z;

	m_LastValidLoc = m_Cam.getLocation();
	m_LastValidAABB = m_Aabb;
}

AABB Player::createPlayerAABB(glm::vec3 playerPos)
{
	AABB aabb{};
	// - 0.35f to make player's head appear higher
	aabb.min(glm::vec3{ playerPos.x - constants::playerSize, playerPos.y - (3.0f * constants::playerSize) - 0.35f, playerPos.z - constants::playerSize });
	aabb.max(glm::vec3{ playerPos.x + constants::playerSize, playerPos.y + constants::playerSize - 0.35f, playerPos.z + constants::playerSize });
	return aabb;
}

bool Player::testCollide(glm::vec3 playerLowerHalf, glm::vec3 playerUpperHalf, AABB& playerAABB, Vector3i& o_Pos, CollsionType& o_CollisionType)
{
	Vector3i lowerBlock{ static_cast<int>(playerLowerHalf.x), static_cast<int>(playerLowerHalf.y), static_cast<int>(playerLowerHalf.z) };
	Vector3i upperBlock{ static_cast<int>(playerUpperHalf.x), static_cast<int>(playerUpperHalf.y), static_cast<int>(playerUpperHalf.z) };

	if (playerLowerHalf.x < 0.0f)
		--lowerBlock.x;
	if (playerLowerHalf.y < 0.0f)
		--lowerBlock.y;
	if (playerLowerHalf.z < 0.0f)
		--lowerBlock.z;

	if (playerUpperHalf.x < 0.0f)
		--upperBlock.x;
	if (playerUpperHalf.y < 0.0f)
		--upperBlock.y;
	if (playerUpperHalf.z < 0.0f)
		--upperBlock.z;

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
				Block block{ m_Manager.getWorldBlock(Vector3i{ x, y, z }) };
				if (blockAABB.intersects(playerAABB) && (block.getType() != BlockType::Air && block.getType() != BlockType::Water))
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
				Block block{ m_Manager.getWorldBlock(Vector3i{ x, y, z }) };
				if (blockAABB.intersects(playerAABB) && (block.getType() != BlockType::Air && block.getType() != BlockType::Water))
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
		m_Velocity.x += constants::playerDrift * Application::s_Dt;
		if (m_Velocity.x > 0.0f)
			m_Velocity.x = 0.0f;
	}
	else
	{
		m_Velocity.x -= constants::playerDrift * Application::s_Dt;
		if (m_Velocity.x < 0.0f)
			m_Velocity.x = 0.0f;
	}

	if (m_Velocity.y < 0.0f)
	{
		m_Velocity.y += constants::playerDrift * Application::s_Dt;
		if (m_Velocity.y > 0.0f)
			m_Velocity.y = 0.0f;
	}
	else
	{
		m_Velocity.y -= constants::playerDrift * Application::s_Dt;
		if (m_Velocity.y < 0.0f)
			m_Velocity.y = 0.0f;
	}

	if (m_Velocity.z < 0.0f)
	{
		m_Velocity.z += constants::playerDrift * Application::s_Dt;
		if (m_Velocity.z > 0.0f)
			m_Velocity.z = 0.0f;
	}
	else
	{
		m_Velocity.z -= constants::playerDrift * Application::s_Dt;
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

		if (!m_Manager.chunkExsists(pos))
			return;

		m_Manager.setWorldBlock(pos, BlockType::Air, false);

		updateMeshes(pos);

		delete breakPos;
	}
}

void Player::placeBlock(BlockType type)
{
	glm::vec3 camFront{ m_Cam.getFront() * vecPrecision };
	glm::vec3* intersect{ placeIntersect() };

	if (intersect != nullptr)
	{
		glm::vec3 pos{ *intersect };
		Vector3i blockPos{ static_cast<int>(intersect->x < 0.0f ? intersect->x - 1.0f : intersect->x), static_cast<int>(intersect->y < 0.0f ? intersect->y - 1.0f : intersect->y), static_cast<int>(intersect->z < 0.0f ? intersect->z - 1.0f : intersect->z) };
		Vector3i placePos{ static_cast<int>(intersect->x < 0.0f ? pos.x - 1.0f : pos.x), static_cast<int>(intersect->y < 0.0f ? pos.y - 1.0f : pos.y), static_cast<int>(intersect->z < 0.0f ? pos.z - 1.0f : pos.z) };
		while (placePos == blockPos)
		{
			pos -= camFront;
			placePos.x = static_cast<int>(pos.x < 0.0f ? pos.x - 1.0f : pos.x);
			placePos.y = static_cast<int>(pos.y < 0.0f ? pos.y - 1.0f : pos.y);
			placePos.z = static_cast<int>(pos.z < 0.0f ? pos.z - 1.0f : pos.z);
		}

		if (pos.y < 0.0f)
			return;

		if (!m_Manager.chunkExsists(placePos))
			return;

		AABB blockAABB{ glm::vec3{ placePos.x, placePos.y, placePos.z }, glm::vec3{ placePos.x + 1, placePos.y + 1, placePos.z + 1 } };
		if (m_Aabb.intersects(blockAABB))
			return;

		m_Manager.setWorldBlock(placePos, type, false);

		updateMeshes(placePos);

		delete intersect;
	}
}

void Player::updateMeshes(Vector3i editPos)
{
	Chunk* chunk{ m_Manager.getChunk(editPos) };

	chunk->clearMesh();
	for (int i{}; i < 16; ++i)
	{
		chunk->buildMesh(m_Manager, i);
	}

	Vector3i local{ m_Manager.toSectionLocal(editPos) };

	if (local.x == 15)
	{
		Chunk* chunkPosX{ m_Manager.getChunk(Vector2i{ chunk->getLocation().x + 1, chunk->getLocation().y }) };

		chunkPosX->clearMesh();
		for (int i{}; i < 16; ++i)
		{
			chunkPosX->buildMesh(m_Manager, i);
		}
	}

	if (local.x == 0)
	{
		Chunk* chunkNegX{ m_Manager.getChunk(Vector2i{ chunk->getLocation().x - 1, chunk->getLocation().y }) };

		chunkNegX->clearMesh();
		for (int i{}; i < 16; ++i)
		{
			chunkNegX->buildMesh(m_Manager, i);
		}
	}

	if (local.z == 15)
	{
		Chunk* chunkPosY{ m_Manager.getChunk(Vector2i{ chunk->getLocation().x, chunk->getLocation().y + 1 }) };

		chunkPosY->clearMesh();
		for (int i{}; i < 16; ++i)
		{
			chunkPosY->buildMesh(m_Manager, i);
		}
	}

	if (local.z == 0)
	{
		Chunk* chunkNegY{ m_Manager.getChunk(Vector2i{ chunk->getLocation().x, chunk->getLocation().y - 1 }) };

		chunkNegY->clearMesh();
		for (int i{}; i < 16; ++i)
		{
			chunkNegY->buildMesh(m_Manager, i);
		}
	}
}

Vector3i* Player::breakIntersect()
{
	glm::vec3 camFront{ m_Cam.getFront() * vecPrecision };
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

		if (m_Manager.getWorldBlock(blockPos).getType() != BlockType::Air && m_Manager.getWorldBlock(blockPos).getType() != BlockType::Water)
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
	glm::vec3 camFront{ m_Cam.getFront() * vecPrecision };
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

		if (m_Manager.getWorldBlock(blockPos).getType() != BlockType::Air && m_Manager.getWorldBlock(blockPos).getType() != BlockType::Water)
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