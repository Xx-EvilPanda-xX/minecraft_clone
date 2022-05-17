#include "Player.h"
#include "../World/Chunk.h"
#include <glm/vec3.hpp>
#include "../Application.h"
#include "../Constants.h"
#include <cmath>

Player::Player(ChunkManager& manager, Keyboard& keyboard, double reach)
	: m_Camera{ constants::playerSpawnPos, 0.0, 0.0, constants::cameraFov, constants::mouse_sensitivity },
	m_Manager{ manager },
	m_Keyboard{ keyboard },
	m_Reach{ reach },
	m_Velocity{},
	m_LastValidLoc{},
	m_LastValidAABB{}
{
	m_JumpCoolDown = 0.25;
	m_Sprinting = false;
	m_Grounded = false;
	m_Flying = false;
	m_LastMovedX = false;
	m_LastMovedY = false;
	m_LastMovedZ = false;
	m_CtrlLastDown = false;
	m_DecreasingVel = false;
}

void Player::move()
{
	calculateVelocity();

	//gravity
	if (!m_Flying && !m_Grounded)
		m_Velocity.y -= constants::gravity * Application::s_Dt;
	
	m_Camera.handleMove(m_Velocity, Application::s_Dt);
	m_Aabb = createPlayerAABB(m_Camera.getLocation());
	
	collsionDetection();

	m_JumpCoolDown -= Application::s_Dt;

	m_LastMovedX = m_LastValidLoc.x != m_Camera.getLocation().x;
	m_LastMovedY = m_LastValidLoc.y != m_Camera.getLocation().y;
	m_LastMovedZ = m_LastValidLoc.z != m_Camera.getLocation().z;

	m_LastValidLoc = m_Camera.getLocation();
	m_LastValidAABB = m_Aabb;
	m_CtrlLastDown = m_Keyboard.isKeyDown(GLFW_KEY_LEFT_CONTROL);

	if (m_Camera.getLocation().y <= worldBottom)
	{
		m_Camera.setY(worldBottom);
		m_Velocity.y = 0.0;
		m_Grounded = true;
	}
}

void Player::collsionDetection()
{
	glm::dvec3 lowerPlayerHalf{ m_Camera.getLocation().x, m_Aabb.min().y + constants::playerSize, m_Camera.getLocation().z };
	glm::dvec3 upperPlayerHalf{ m_Camera.getLocation().x, m_Aabb.max().y - constants::playerSize, m_Camera.getLocation().z };

	bool onGround{ false };
	CollsionType collisionType{};
	Vector3i collsionPos{};

	//3 iters, one per axis
	for (int i{}; i < 3; ++i)
	{
		if (collide(lowerPlayerHalf, upperPlayerHalf, m_Aabb, collsionPos, collisionType))
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
			glm::dvec3 testLoc{ blockCenter };
			Vector3i block{ testLoc };
			while (block == collsionPos)
			{
				testLoc += direction;
				block = Vector3i{ testLoc };
			}

			bool collideX{ block.x != collsionPos.x };
			bool collideY{ block.y != collsionPos.y };
			bool collideZ{ block.z != collsionPos.z };
			int intersects{};

			if (collideX) ++intersects;
			if (collideY) ++intersects;
			if (collideZ) ++intersects;

			if (intersects != 1)
			{
				collideX = false;
				collideY = false;
				collideZ = false;

				if (!m_LastMovedX && m_LastMovedY && m_LastMovedZ)
				{
					if (blockCenter.y < m_Aabb.min().y || blockCenter.y > m_Aabb.max().y)
						collideY = true;
					else
						collideX = true;
				}


				if (m_LastMovedX && !m_LastMovedY && m_LastMovedZ)
				{
					if (blockCenter.y > m_Aabb.min().y && blockCenter.y < m_Aabb.max().y)
						collideX = true;
					else
						collideY = true;
				}

				if (m_LastMovedX && m_LastMovedY && !m_LastMovedZ)
				{
					if (blockCenter.y < m_Aabb.min().y || blockCenter.y > m_Aabb.max().y)
						collideY = true;
					else
						collideZ = true;
				}

				if (!m_LastMovedX && !m_LastMovedY && m_LastMovedZ)
				{
					if (blockCenter.y > m_Aabb.min().y && blockCenter.y < m_Aabb.max().y)
						collideX = true;
					else
						collideY = true;
				}


				if (m_LastMovedX && !m_LastMovedY && !m_LastMovedZ)
				{
					if (blockCenter.y > m_Aabb.min().y && blockCenter.y < m_Aabb.max().y)
						collideZ = true;
					else
						collideY = true;
				}

				if (!m_LastMovedX && m_LastMovedY && !m_LastMovedZ)
				{
					if (blockCenter.y > m_Aabb.min().y && blockCenter.y < m_Aabb.max().y)
						collideX = true;
					else
						collideY = true;
				}

				if (m_LastMovedX && m_LastMovedY && m_LastMovedZ)
				{
					if (blockCenter.y > m_Aabb.min().y && blockCenter.y < m_Aabb.max().y)
						collideX = true;
					else
						collideY = true;
				}
			}

			constexpr double targetDistance{ 0.5 + constants::playerSize + 0.0000001 };

			if (collideX)
				m_Camera.setX(blockCenter.x < lastValidLoc.x ? (blockCenter.x + targetDistance) : (blockCenter.x - targetDistance));

			if (collideY)
			{
				m_Camera.setY(blockCenter.y < lastValidLoc.y ? (blockCenter.y + targetDistance) + (constants::playerSize * 2.0) + cameraHeightDiff : (blockCenter.y - targetDistance) + cameraHeightDiff);
				m_Velocity.y = 0.0;

				if (collisionType == CollsionType::PlayerLowerHalf && m_JumpCoolDown <= 0.0)
				{
					onGround = true;
					m_JumpCoolDown = 0.0;
					if (m_Keyboard.isKeyDown(GLFW_KEY_SPACE))
						m_JumpCoolDown = 0.15;
				}
			}

			if (collideZ)
				m_Camera.setZ(blockCenter.z < lastValidLoc.z ? (blockCenter.z + targetDistance) : (blockCenter.z - targetDistance));

			m_Aabb = createPlayerAABB(m_Camera.getLocation());

			lowerPlayerHalf = { m_Camera.getLocation().x, m_Aabb.min().y + constants::playerSize, m_Camera.getLocation().z };
			upperPlayerHalf = { m_Camera.getLocation().x, m_Aabb.max().y - constants::playerSize, m_Camera.getLocation().z };
		}
	}

	m_Grounded = onGround;
}

AABB Player::createPlayerAABB(glm::dvec3 playerPos)
{
	AABB aabb{};
	// - 0.35f to make player's head appear higher
	aabb.min(glm::dvec3{ playerPos.x - constants::playerSize, playerPos.y - (3.0 * constants::playerSize) - cameraHeightDiff, playerPos.z - constants::playerSize });
	aabb.max(glm::dvec3{ playerPos.x + constants::playerSize, playerPos.y + constants::playerSize - cameraHeightDiff, playerPos.z + constants::playerSize });
	return aabb;
}

bool Player::collide(glm::dvec3 playerLowerHalf, glm::dvec3 playerUpperHalf, const AABB& playerAABB, Vector3i& o_Pos, CollsionType& o_CollisionType)
{
	double lowerClosestCollision{};
	Vector3i* lowerCollsionPos{ test(playerLowerHalf, playerAABB, lowerClosestCollision) };

	double upperClosestCollision{};
	Vector3i* upperCollsionPos{ test(playerUpperHalf, playerAABB, upperClosestCollision) };

	bool collision{};

	if (lowerCollsionPos && upperCollsionPos)
	{
		if (lowerClosestCollision <= upperClosestCollision)
		{
			o_Pos = *lowerCollsionPos;
			o_CollisionType = CollsionType::PlayerLowerHalf;
		}
		if (upperClosestCollision <= lowerClosestCollision)
		{
			o_Pos = *upperCollsionPos;
			o_CollisionType = CollsionType::PlayerUpperHalf;
		}
		collision = true;
	}

	if (lowerCollsionPos) delete lowerCollsionPos;
	if (upperCollsionPos) delete upperCollsionPos;
	return collision;
}

Vector3i* Player::test(glm::dvec3 playerPos, const AABB& playerAABB, double& o_ClosestCollision)
{
	Vector3i playerBlockPos{ playerPos };
	bool collision{ false };
	double closestCollision{ -1.0 };
	Vector3i* collisionPos{ nullptr };

	for (int x{ playerBlockPos.x - collsionSearchRadiusX }; x < playerBlockPos.x + collsionSearchRadiusX; ++x)
	{
		for (int y{ playerBlockPos.y - collsionSearchRadiusY }; y < playerBlockPos.y + collsionSearchRadiusY; ++y)
		{
			for (int z{ playerBlockPos.z - collsionSearchRadiusZ }; z < playerBlockPos.z + collsionSearchRadiusZ; ++z)
			{
				AABB blockAABB{ glm::dvec3{ x, y, z }, glm::dvec3{ x + 1, y + 1, z + 1 } };
				Block block{ m_Manager.getWorldBlock(Vector3i{ x, y, z }) };
				if (blockAABB.intersects(playerAABB) && (block.getType() != BlockType::Air && block.getType() != BlockType::Water))
				{
					glm::dvec3 blockCenter{ x + 0.5, y + 0.5, z + 0.5 };
					glm::dvec3 playerToCenter{ blockCenter - playerPos };

					double distance{ glm::length(playerToCenter) };
					collision = true;
					if (distance < closestCollision || closestCollision == -1.0)
					{
						closestCollision = distance;
						if (collisionPos) delete collisionPos;
						collisionPos = new Vector3i{ x, y, z };
					}
				}
			}
		}
	}

	o_ClosestCollision = closestCollision;
	return collisionPos;
}

void Player::calculateVelocity()
{
	if (!m_DecreasingVel)
		m_DecreasingVel = m_CtrlLastDown && !m_Keyboard.isKeyDown(GLFW_KEY_LEFT_CONTROL);

	if (!m_Keyboard.isKeyDown(GLFW_KEY_A) || m_DecreasingVel)
	{
		if (m_Velocity.x < 0.0)
		{
			m_Velocity.x += constants::playerDrift * Application::s_Dt;
			if (m_Velocity.x > 0.0) m_Velocity.x = 0.0;
		}
	}

	if (!m_Keyboard.isKeyDown(GLFW_KEY_D) || m_DecreasingVel)
	{
		if (m_Velocity.x > 0.0)
		{
			m_Velocity.x -= constants::playerDrift * Application::s_Dt;
			if (m_Velocity.x < 0.0) m_Velocity.x = 0.0;
		}
	}

	if (!m_Keyboard.isKeyDown(GLFW_KEY_LEFT_SHIFT) || m_DecreasingVel)
	{
		if (m_Velocity.y < 0.0)
		{
			m_Velocity.y += constants::playerDrift * Application::s_Dt;
			if (m_Velocity.y > 0.0) m_Velocity.y = 0.0;
		}
	}

	if (!m_Keyboard.isKeyDown(GLFW_KEY_SPACE) || m_DecreasingVel)
	{
		if (m_Velocity.y > 0.0)
		{
			m_Velocity.y -= constants::playerDrift * Application::s_Dt;
			if (m_Velocity.y < 0.0) m_Velocity.y = 0.0;
		}
	}

	if (!m_Keyboard.isKeyDown(GLFW_KEY_S) || m_DecreasingVel)
	{
		if (m_Velocity.z < 0.0)
		{
			m_Velocity.z += constants::playerDrift * Application::s_Dt;
			if (m_Velocity.z > 0.0) m_Velocity.z = 0.0;
		}
	}

	if (!m_Keyboard.isKeyDown(GLFW_KEY_W) || m_DecreasingVel)
	{
		if (m_Velocity.z > 0.0)
		{
			m_Velocity.z -= constants::playerDrift * Application::s_Dt;
			if (m_Velocity.z < 0.0) m_Velocity.z = 0.0;
		}
	}

	double speed{ constants::walkSpeed };
	if ((m_Velocity.x <= speed && m_Velocity.y <= speed && m_Velocity.z <= speed) && (m_Velocity.x >= -speed && m_Velocity.y >= -speed && m_Velocity.z >= -speed))
		m_DecreasingVel = false;

	if ((m_Velocity.x < 0.01 && m_Velocity.x > 0.0) || (m_Velocity.x > -0.01 && m_Velocity.x < 0.0))
		m_Velocity.x = 0.0;
	if ((m_Velocity.y < 0.01 && m_Velocity.y > 0.0) || (m_Velocity.y > -0.01 && m_Velocity.y < 0.0))
		m_Velocity.y = 0.0;
	if ((m_Velocity.z < 0.01 && m_Velocity.z > 0.0) || (m_Velocity.z > -0.01 && m_Velocity.z < 0.0))
		m_Velocity.z = 0.0;
}

void Player::breakBlock()
{
	Vector3i* breakPos{ intersect<Vector3i>() };

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
	glm::dvec3 camFront{ m_Camera.getFront() * vecPrecision };
	glm::dvec3* inter{ intersect<glm::dvec3>() };

	if (inter != nullptr)
	{
		glm::dvec3 pos{ *inter };
		Vector3i blockPos{ *inter };
		Vector3i placePos{ pos };
		while (placePos == blockPos)
		{
			pos -= camFront;
			placePos = Vector3i{ pos };
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

		delete inter;
	}
}

template <typename T>
T* Player::intersect()
{
	glm::dvec3 camFront{ m_Camera.getFront() * vecPrecision };
	glm::dvec3 currentPos{ m_Camera.getLocation() };
	T* intersectPos{ nullptr };

	while (glm::length((m_Camera.getLocation() - currentPos)) < m_Reach)
	{
		Vector3i blockPos{ currentPos };
		if (m_Manager.getWorldBlock(blockPos).getType() != BlockType::Air && m_Manager.getWorldBlock(blockPos).getType() != BlockType::Water)
		{
			intersectPos = new T{ currentPos };
			break;
		}

		currentPos += camFront;
	}

	return intersectPos;
}

void Player::updateMeshes(Vector3i editPos)
{
	Chunk* chunk{ m_Manager.getChunk(editPos) };

	if (!chunk->isBuilt())
		return;

	int chunkIndex;
	if (m_Manager.isInBuildQueue(chunk, chunkIndex))
		m_Manager.getBuildQueue().erase(m_Manager.getBuildQueue().begin() + chunkIndex);

	chunk->clearMesh();
	Chunk* adjacentChunks[4]{};
	m_Manager.getAdjacentChunks(chunk->getLocation(), adjacentChunks);
	for (int i{}; i < 16; ++i)
	{
		chunk->buildMesh(m_Manager, i, adjacentChunks);
	}

	Vector3i local{ m_Manager.toSectionLocal(editPos) };

	if (local.x == 15)
	{
		Chunk* chunkPosX{ m_Manager.getChunk(Vector2i{ chunk->getLocation().x + 1, chunk->getLocation().y }) };

		if (m_Manager.isInBuildQueue(chunkPosX, chunkIndex))
			m_Manager.getBuildQueue().erase(m_Manager.getBuildQueue().begin() + chunkIndex);

		chunkPosX->clearMesh();
		m_Manager.getAdjacentChunks(chunkPosX->getLocation(), adjacentChunks);
		for (int i{}; i < 16; ++i)
		{
			chunkPosX->buildMesh(m_Manager, i, adjacentChunks);
		}
	}

	if (local.x == 0)
	{
		Chunk* chunkNegX{ m_Manager.getChunk(Vector2i{ chunk->getLocation().x - 1, chunk->getLocation().y }) };

		if (m_Manager.isInBuildQueue(chunkNegX, chunkIndex))
			m_Manager.getBuildQueue().erase(m_Manager.getBuildQueue().begin() + chunkIndex);

		chunkNegX->clearMesh();
		m_Manager.getAdjacentChunks(chunkNegX->getLocation(), adjacentChunks);
		for (int i{}; i < 16; ++i)
		{
			chunkNegX->buildMesh(m_Manager, i, adjacentChunks);
		}
	}

	if (local.z == 15)
	{
		Chunk* chunkPosY{ m_Manager.getChunk(Vector2i{ chunk->getLocation().x, chunk->getLocation().y + 1 }) };

		if (m_Manager.isInBuildQueue(chunkPosY, chunkIndex))
			m_Manager.getBuildQueue().erase(m_Manager.getBuildQueue().begin() + chunkIndex);

		chunkPosY->clearMesh();
		m_Manager.getAdjacentChunks(chunkPosY->getLocation(), adjacentChunks);
		for (int i{}; i < 16; ++i)
		{
			chunkPosY->buildMesh(m_Manager, i, adjacentChunks);
		}
	}

	if (local.z == 0)
	{
		Chunk* chunkNegY{ m_Manager.getChunk(Vector2i{ chunk->getLocation().x, chunk->getLocation().y - 1 }) };

		if (m_Manager.isInBuildQueue(chunkNegY, chunkIndex))
			m_Manager.getBuildQueue().erase(m_Manager.getBuildQueue().begin() + chunkIndex);

		chunkNegY->clearMesh();
		m_Manager.getAdjacentChunks(chunkNegY->getLocation(), adjacentChunks);
		for (int i{}; i < 16; ++i)
		{
			chunkNegY->buildMesh(m_Manager, i, adjacentChunks);
		}
	}
}

double Player::getReach() const
{
	return m_Reach;
}

void Player::setReach(double reach)
{
	m_Reach = reach;
}

Camera& Player::getCamera()
{
	return m_Camera;
}

glm::dvec3& Player::getVelocity()
{
	return m_Velocity;
}

bool Player::isSprinting() const
{
	return m_Sprinting;
}

bool Player::isGrounded() const
{
	return m_Grounded;
}

bool Player::isFlying() const
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