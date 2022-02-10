#include "Player.h"
#include "../World/Chunk.h"
#include <glm/vec3.hpp>
#include "../Application.h"
#include "../Constants.h"
#include <stack>

Player::Player(Camera & cam, ChunkManager & manager, double reach)
	: m_Cam{ cam },
	m_Manager{ manager },
	m_Reach{ reach }
{
	m_Velocity = glm::dvec3{};
	m_LastValidLoc = glm::dvec3{};

	m_Sprinting = false;
	m_Grounded = false;
	m_Flying = false;
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

	glm::dvec3 lowerPlayerHalf{ m_Cam.getLocation().x, m_Aabb.min().y + constants::playerSize, m_Cam.getLocation().z };
	glm::dvec3 upperPlayerHalf{ m_Cam.getLocation().x, m_Aabb.max().y - constants::playerSize, m_Cam.getLocation().z };

	CollsionType collisionType{};

	//3 iters, one per axis
	for (int i{}; testCollide(lowerPlayerHalf, upperPlayerHalf, m_Aabb, collsionPos, collisionType) && i < 3; ++i)
	{
		glm::dvec3 blockCenter{ collsionPos.x + 0.5, collsionPos.y + 0.5, collsionPos.z + 0.5 };
		glm::dvec3 lastValidLoc;

		switch (collisionType)
		{
		case CollsionType::PlayerUpperHalf:
			lastValidLoc = { m_LastValidLoc.x, m_LastValidAABB.max().y - constants::playerSize, m_LastValidLoc.z };
			break;
		case CollsionType::PlayerLowerHalf:
			lastValidLoc = { m_LastValidLoc.x, m_LastValidAABB.min().y + constants::playerSize, m_LastValidLoc.z };
			break;
		}

		glm::dvec3 direction{ glm::normalize(lastValidLoc - blockCenter) * vecPrecision };
		glm::dvec3 center{ blockCenter };
		Vector3i block{ static_cast<int>(center.x < 0.0 ? center.x - 1.0 : center.x), static_cast<int>(center.y < 0.0 ? center.y - 1.0 : center.y), static_cast<int>(center.z < 0.0 ? center.z - 1.0 : center.z) };
		while (block == collsionPos)
		{
			center += direction;
			block.x = static_cast<int>(center.x < 0.0 ? center.x - 1.0 : center.x);
			block.y = static_cast<int>(center.y < 0.0 ? center.y - 1.0 : center.y);
			block.z = static_cast<int>(center.z < 0.0 ? center.z - 1.0 : center.z);
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

		glm::dvec3 camPos{ m_Cam.getLocation() };
		const double targetDistance{ 0.5 + constants::playerSize + 0.0001 };

		if (collideX)
		{
			double centerToPlayerDistance{ std::abs(blockCenter.x - lastValidLoc.x) };
			double change{ centerToPlayerDistance - targetDistance };

			if (change > 0.1)
				std::cout << "pum\n";

			m_Cam.setLocation(glm::dvec3{ m_LastValidLoc.x + (blockCenter.x < lastValidLoc.x ? -change : change), camPos.y, camPos.z });
		}

		if (collideY)
		{
			double centerToPlayerDistance{ std::abs(blockCenter.y - lastValidLoc.y) };
			double change{ centerToPlayerDistance - targetDistance };

			if (change > 0.1)
				std::cout << "pum\n";

			m_Cam.setLocation(glm::dvec3{ camPos.x, m_LastValidLoc.y + (blockCenter.y < lastValidLoc.y ? -change : change), camPos.z });
			m_Velocity.y = 0.0;

			if (collisionType == CollsionType::PlayerLowerHalf)
				onGround = true;
		}

		if (collideZ)
		{
			double centerToPlayerDistance{ std::abs(blockCenter.z - lastValidLoc.z) };
			double change{ centerToPlayerDistance - targetDistance };

			if (change > 0.1)
				std::cout << "pum\n";

			m_Cam.setLocation(glm::dvec3{ camPos.x, camPos.y, m_LastValidLoc.z + (blockCenter.z < lastValidLoc.z ? -change : change) });
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

AABB Player::createPlayerAABB(glm::dvec3 playerPos)
{
	AABB aabb{};
	// - 0.35f to make player's head appear higher
	aabb.min(glm::dvec3{ playerPos.x - constants::playerSize, playerPos.y - (3.0 * constants::playerSize) - 0.35, playerPos.z - constants::playerSize });
	aabb.max(glm::dvec3{ playerPos.x + constants::playerSize, playerPos.y + constants::playerSize - 0.35, playerPos.z + constants::playerSize });
	return aabb;
}

bool Player::testCollide(glm::dvec3 playerLowerHalf, glm::dvec3 playerUpperHalf, AABB& playerAABB, Vector3i& o_Pos, CollsionType& o_CollisionType)
{
	Vector3i lowerBlock{ static_cast<int>(playerLowerHalf.x), static_cast<int>(playerLowerHalf.y), static_cast<int>(playerLowerHalf.z) };
	Vector3i upperBlock{ static_cast<int>(playerUpperHalf.x), static_cast<int>(playerUpperHalf.y), static_cast<int>(playerUpperHalf.z) };

	if (playerLowerHalf.x < 0.0)
		--lowerBlock.x;
	if (playerLowerHalf.y < 0.0)
		--lowerBlock.y;
	if (playerLowerHalf.z < 0.0)
		--lowerBlock.z;

	if (playerUpperHalf.x < 0.0)
		--upperBlock.x;
	if (playerUpperHalf.y < 0.0)
		--upperBlock.y;
	if (playerUpperHalf.z < 0.0)
		--upperBlock.z;

	bool collision{ false };

	double lowerClosestCollision{ -1.0 };
	Vector3i lowerCollsionPos{};
	AABB lowerAABB{};

	double upperClosestCollision{ -1.0 };
	Vector3i upperCollsionPos{};
	AABB upperAABB{};

	for (int x{ lowerBlock.x - 2 }; x < lowerBlock.x + 2; ++x)
	{
		for (int y{ lowerBlock.y - 2 }; y < lowerBlock.y + 2; ++y)
		{
			for (int z{ lowerBlock.z - 2 }; z < lowerBlock.z + 2; ++z)
			{
				AABB blockAABB{ glm::dvec3{ x, y, z }, glm::dvec3{ x + 1, y + 1, z + 1 } };
				Block block{ m_Manager.getWorldBlock(Vector3i{ x, y, z }) };
				if (blockAABB.intersects(playerAABB) && (block.getType() != BlockType::Air && block.getType() != BlockType::Water))
				{
					glm::dvec3 blockCenter{ x + 0.5, y + 0.5, z + 0.5 };
					glm::dvec3 playerToCenter{ blockCenter - playerLowerHalf };
					double distance{ glm::length(playerToCenter) };

					collision = true;

					if (distance < lowerClosestCollision || lowerClosestCollision == -1.0)
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
				AABB blockAABB{ glm::dvec3{ x, y, z }, glm::dvec3{ x + 1, y + 1, z + 1 } };
				Block block{ m_Manager.getWorldBlock(Vector3i{ x, y, z }) };
				if (blockAABB.intersects(playerAABB) && (block.getType() != BlockType::Air && block.getType() != BlockType::Water))
				{
					glm::dvec3 blockCenter{ x + 0.5, y + 0.5, z + 0.5 };
					glm::dvec3 playerToCenter{ blockCenter - playerUpperHalf };
					double distance{ glm::length(playerToCenter) };

					collision = true;

					if (distance < upperClosestCollision || upperClosestCollision == -1.0)
					{
						upperClosestCollision = distance;
						upperCollsionPos = { x, y, z };
						upperAABB = blockAABB;
					}
				}
			}
		}
	}

	if (lowerClosestCollision != -1.0)
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

	else if (upperClosestCollision != -1.0)
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
	if (m_Velocity.x < 0.0)
	{
		m_Velocity.x += constants::playerDrift * Application::s_Dt;
		if (m_Velocity.x > 0.0)
			m_Velocity.x = 0.0;
	}
	else
	{
		m_Velocity.x -= constants::playerDrift * Application::s_Dt;
		if (m_Velocity.x < 0.0)
			m_Velocity.x = 0.0;
	}

	if (m_Velocity.y < 0.0)
	{
		m_Velocity.y += constants::playerDrift * Application::s_Dt;
		if (m_Velocity.y > 0.0)
			m_Velocity.y = 0.0;
	}
	else
	{
		m_Velocity.y -= constants::playerDrift * Application::s_Dt;
		if (m_Velocity.y < 0.0)
			m_Velocity.y = 0.0;
	}

	if (m_Velocity.z < 0.0)
	{
		m_Velocity.z += constants::playerDrift * Application::s_Dt;
		if (m_Velocity.z > 0.0)
			m_Velocity.z = 0.0;
	}
	else
	{
		m_Velocity.z -= constants::playerDrift * Application::s_Dt;
		if (m_Velocity.z < 0.0)
			m_Velocity.z = 0.0;
	}

	if ((m_Velocity.x < 0.01 && m_Velocity.x > 0.0) || (m_Velocity.x > -0.01 && m_Velocity.x < 0.0))
		m_Velocity.x = 0.0;
	if ((m_Velocity.y < 0.01 && m_Velocity.y > 0.0) || (m_Velocity.y > -0.01 && m_Velocity.y < 0.0))
		m_Velocity.y = 0.0;
	if ((m_Velocity.z < 0.01 && m_Velocity.z > 0.0) || (m_Velocity.z > -0.01 && m_Velocity.z < 0.0))
		m_Velocity.z = 0.0;
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
	glm::dvec3 camFront{ m_Cam.getFront() * vecPrecision };
	glm::dvec3* intersect{ placeIntersect() };

	if (intersect != nullptr)
	{
		glm::dvec3 pos{ *intersect };
		Vector3i blockPos{ static_cast<int>(intersect->x < 0.0 ? intersect->x - 1.0 : intersect->x), static_cast<int>(intersect->y < 0.0 ? intersect->y - 1.0 : intersect->y), static_cast<int>(intersect->z < 0.0 ? intersect->z - 1.0 : intersect->z) };
		Vector3i placePos{ static_cast<int>(intersect->x < 0.0 ? pos.x - 1.0f : pos.x), static_cast<int>(intersect->y < 0.0 ? pos.y - 1.0 : pos.y), static_cast<int>(intersect->z < 0.0 ? pos.z - 1.0 : pos.z) };
		while (placePos == blockPos)
		{
			pos -= camFront;
			placePos.x = static_cast<int>(pos.x < 0.0 ? pos.x - 1.0 : pos.x);
			placePos.y = static_cast<int>(pos.y < 0.0 ? pos.y - 1.0 : pos.y);
			placePos.z = static_cast<int>(pos.z < 0.0 ? pos.z - 1.0 : pos.z);
		}

		if (pos.y < 0.0)
			return;

		if (!m_Manager.chunkExsists(placePos))
			return;

		AABB blockAABB{ glm::dvec3{ placePos.x, placePos.y, placePos.z }, glm::dvec3{ placePos.x + 1, placePos.y + 1, placePos.z + 1 } };
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
	glm::dvec3 camFront{ m_Cam.getFront() * vecPrecision };
	glm::dvec3 currentPos{ m_Cam.getLocation() };
	Vector3i* breakPos{ nullptr };

	while (glm::length((m_Cam.getLocation() - currentPos)) < m_Reach)
	{
		Vector3i blockPos{ static_cast<int>(currentPos.x), static_cast<int>(currentPos.y), static_cast<int>(currentPos.z) };
		if (currentPos.x < 0.0)
			blockPos.x -= 1.0;

		if (currentPos.y < 0.0)
			blockPos.y -= 1.0;

		if (currentPos.z < 0.0)
			blockPos.z -= 1.0;

		if (m_Manager.getWorldBlock(blockPos).getType() != BlockType::Air && m_Manager.getWorldBlock(blockPos).getType() != BlockType::Water)
		{
			breakPos = new Vector3i{ blockPos };
			break;
		}

		currentPos += camFront;
	}

	return breakPos;
}

glm::dvec3* Player::placeIntersect()
{
	glm::dvec3 camFront{ m_Cam.getFront() * vecPrecision };
	glm::dvec3 currentPos{ m_Cam.getLocation() };
	glm::dvec3* placePos{ nullptr };

	while (glm::length((m_Cam.getLocation() - currentPos)) < m_Reach)
	{
		Vector3i blockPos{ static_cast<int>(currentPos.x), static_cast<int>(currentPos.y), static_cast<int>(currentPos.z) };
		if (currentPos.x < 0.0)
			blockPos.x -= 1.0;

		if (currentPos.y < 0.0)
			blockPos.y -= 1.0;

		if (currentPos.z < 0.0)
			blockPos.z -= 1.0;

		if (m_Manager.getWorldBlock(blockPos).getType() != BlockType::Air && m_Manager.getWorldBlock(blockPos).getType() != BlockType::Water)
		{
			placePos = new glm::dvec3{ currentPos };
			break;
		}

		currentPos += camFront;
	}

	return placePos;
}

double Player::getReach() const
{
	return m_Reach;
}

void Player::setReach(double reach)
{
	m_Reach = reach;
}

glm::dvec3& Player::getVelocity()
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