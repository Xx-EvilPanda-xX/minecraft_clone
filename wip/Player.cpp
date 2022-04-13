#include "Player.h"
#include "../World/Chunk.h"
#include <glm/vec3.hpp>
#include "../Application.h"
#include "../Constants.h"
#include <cmath>
#include <limits>

Player::Player(Camera & cam, ChunkManager & manager, Keyboard& keyboard, double reach)
	: m_Cam{ cam },
	m_Manager{ manager },
	m_Keyboard{ keyboard },
	m_Reach{ reach }
{
	m_Velocity = glm::dvec3{};
	m_LastValidLoc = glm::dvec3{};

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
		m_Velocity.y -= Application::s_Dt * constants::gravity;
	
	m_Cam.handleMove(m_Velocity, Application::s_Dt);
	m_Aabb = createPlayerAABB(m_Cam.getLocation());

	bool onGround{ false };
	Vector3i* collsionPos{ nullptr };
	AABB blockAABB{};
	glm::dvec3 playerPos{ m_Cam.getLocation().x, m_Aabb.min().y + ((m_Aabb.max().y - m_Aabb.min().y) / 2.0), m_Cam.getLocation().z };

	//3 iters, one per axis
	for (int i{}; i < 3; ++i)
	{
		collsionPos = test(playerPos, m_Aabb, blockAABB);
		if (collsionPos)
		{
			int iterations[6]{};
			for (int j{}; j < 6; ++j)
			{
				int k{};
				glm::dvec3 playerLoc{ m_Cam.getLocation() };
				AABB playerAABB{ createPlayerAABB(playerLoc) };
				while (blockAABB.intersects(playerAABB))
				{
					switch (j)
					{
					case 0:
						playerLoc.x += collisionPrecision;
						break;
					case 1:
						playerLoc.y += collisionPrecision;
						break;
					case 2:
						playerLoc.z += collisionPrecision;
						break;
					case 3:
						playerLoc.x -= collisionPrecision;
						break;
					case 4:
						playerLoc.y -= collisionPrecision;
						break;
					case 5:
						playerLoc.z -= collisionPrecision;
						break;
					}
					++k;
					playerAABB = createPlayerAABB(playerLoc);
				}

				iterations[j] = k;
			}

			int bestDir{ -1 };
			int secondBestDir{ -1 };
			int tmpIters[6]{};

			for (int j{}; j < 6; ++j)
			{
				if (iterations[j] < iterations[bestDir] || bestDir == -1)
					bestDir = j;
			}

			bool found{};
			for (int j{}; j < 6; ++j)
			{
				if (iterations[j] != iterations[bestDir])
					tmpIters[j] = iterations[j];
				else if (!found)
				{
					tmpIters[j] = INT_MAX;
					found = true;
				}
			}

			for (int j{}; j < 6; ++j)
			{
				if (tmpIters[j] < tmpIters[secondBestDir] || secondBestDir == -1)
					secondBestDir = j;
			}

			bool collideX{ (bestDir == 0 || (secondBestDir == 0 && iterations[bestDir] == iterations[secondBestDir])) || (bestDir == 3 || (secondBestDir == 3 && iterations[bestDir] == iterations[secondBestDir])) };
			bool collideY{ (bestDir == 1 || (secondBestDir == 1 && iterations[bestDir] == iterations[secondBestDir])) || (bestDir == 4 || (secondBestDir == 4 && iterations[bestDir] == iterations[secondBestDir])) };
			bool collideZ{ (bestDir == 2 || (secondBestDir == 2 && iterations[bestDir] == iterations[secondBestDir])) || (bestDir == 5 || (secondBestDir == 5 && iterations[bestDir] == iterations[secondBestDir])) };
			int intersects{};

			if (collideX)
				++intersects;

			if (collideY)
				++intersects;

			if (collideZ)
				++intersects;

			glm::dvec3 blockCenter{ collsionPos->x + 0.5, collsionPos->y + 0.5, collsionPos->z + 0.5 };
			if (intersects != 1)
			{
				collideX = false;
				collideY = false;
				collideZ = false;

				if (!m_LastMovedX && m_LastMovedY && m_LastMovedZ)
				{
					if (blockCenter.y < m_Aabb.min().y || blockCenter.y > m_Aabb.max().y) collideY = true;
					else collideX = true;
				}
					

				if (m_LastMovedX && !m_LastMovedY && m_LastMovedZ)
				{
					if (blockCenter.y > m_Aabb.min().y && blockCenter.y < m_Aabb.max().y) collideX = true;
					else collideY = true;
				}

				if (m_LastMovedX && m_LastMovedY && !m_LastMovedZ)
				{
					if (blockCenter.y < m_Aabb.min().y || blockCenter.y > m_Aabb.max().y) collideY = true;
					else collideZ = true;
				}

				if (!m_LastMovedX && !m_LastMovedY && m_LastMovedZ)
				{
					if (blockCenter.y > m_Aabb.min().y && blockCenter.y < m_Aabb.max().y) collideX = true;
					else collideY = true;
				}
					

				if (m_LastMovedX && !m_LastMovedY && !m_LastMovedZ)
				{
					if (blockCenter.y > m_Aabb.min().y && blockCenter.y < m_Aabb.max().y) collideZ = true;
					else collideY = true;
				}

				if (!m_LastMovedX && m_LastMovedY && !m_LastMovedZ)
				{
					if (blockCenter.y > m_Aabb.min().y && blockCenter.y < m_Aabb.max().y) collideX = true;
					else collideY = true;
				}

				if (m_LastMovedX && m_LastMovedY && m_LastMovedZ)
				{
					if (blockCenter.y > m_Aabb.min().y && blockCenter.y < m_Aabb.max().y) collideX = true;
					else collideY = true;
				}
			}

			const double targetDistance{ 0.5 + constants::playerSize + 0.0000001 };

			if (collideX)
				m_Cam.setX(blockCenter.x < m_LastValidLoc.x ? (blockCenter.x + targetDistance) : (blockCenter.x - targetDistance));

			if (collideY)
			{
				m_Cam.setY(blockCenter.y < m_LastValidLoc.y ? (blockCenter.y + targetDistance) + (constants::playerSize * 2.0) + cameraHeightDiff : (blockCenter.y - targetDistance) + cameraHeightDiff);
				m_Velocity.y = 0.0;

				if (bestDir == 1 && m_JumpCoolDown <= 0.0)
				{
					onGround = true;
					if (m_Keyboard.isKeyDown(GLFW_KEY_SPACE))
						m_JumpCoolDown = 0.25;
				}
			}

			if (collideZ)
				m_Cam.setZ(blockCenter.z < m_LastValidLoc.z ? (blockCenter.z + targetDistance) : (blockCenter.z - targetDistance));

			m_Aabb = createPlayerAABB(m_Cam.getLocation());
		}
		if (collsionPos)
			delete collsionPos;
	}

	m_Grounded = onGround;
	m_JumpCoolDown -= Application::s_Dt;

	m_LastMovedX = m_LastValidLoc.x != m_Cam.getLocation().x;
	m_LastMovedY = m_LastValidLoc.y != m_Cam.getLocation().y;
	m_LastMovedZ = m_LastValidLoc.z != m_Cam.getLocation().z;

	m_LastValidLoc = m_Cam.getLocation();
	m_LastValidAABB = m_Aabb;
	m_CtrlLastDown = m_Keyboard.isKeyDown(GLFW_KEY_LEFT_CONTROL);
}

AABB Player::createPlayerAABB(glm::dvec3 playerPos)
{
	AABB aabb{};
	// - 0.35f to make player's head appear higher
	aabb.min(glm::dvec3{ playerPos.x - constants::playerSize, playerPos.y - (3.0 * constants::playerSize) - cameraHeightDiff, playerPos.z - constants::playerSize });
	aabb.max(glm::dvec3{ playerPos.x + constants::playerSize, playerPos.y + constants::playerSize - cameraHeightDiff, playerPos.z + constants::playerSize });
	return aabb;
}

/*
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
*/

Vector3i* Player::test(glm::dvec3 playerPos, const AABB& playerAABB, AABB& o_BlockAABB)
{
	Vector3i playerBlockPos{ static_cast<int>(playerPos.x), static_cast<int>(playerPos.y), static_cast<int>(playerPos.z) };

	if (playerPos.x < 0.0)
		--playerBlockPos.x;
	if (playerPos.y < 0.0)
		--playerBlockPos.y;
	if (playerPos.z < 0.0)
		--playerBlockPos.z;

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
						o_BlockAABB = blockAABB;
						if (collisionPos) delete collisionPos;
						collisionPos = new Vector3i{ x, y, z };
					}
				}
			}
		}
	}

	return collisionPos;
}

void Player::calculateVelocity()
{
	if (!m_DecreasingVel)
		m_DecreasingVel = m_CtrlLastDown && !m_Keyboard.isKeyDown(GLFW_KEY_LEFT_CONTROL);

	double speed{ constants::walkSpeed };
	if ((m_Velocity.x <= speed && m_Velocity.y <= speed && m_Velocity.z <= speed) && (m_Velocity.x >= -speed && m_Velocity.y >= -speed && m_Velocity.z >= -speed))
		m_DecreasingVel = false;

	if ((!m_Keyboard.isKeyDown(GLFW_KEY_A) && !m_Keyboard.isKeyDown(GLFW_KEY_D)) || m_DecreasingVel)
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
	}
	
	if ((!m_Keyboard.isKeyDown(GLFW_KEY_SPACE) && !m_Keyboard.isKeyDown(GLFW_KEY_LEFT_SHIFT)) || (m_DecreasingVel && m_Flying))
	{
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
	}

	if ((!m_Keyboard.isKeyDown(GLFW_KEY_W) && !m_Keyboard.isKeyDown(GLFW_KEY_S)) || m_DecreasingVel)
	{
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
		Vector3i placePos{ static_cast<int>(intersect->x < 0.0 ? pos.x - 1.0 : pos.x), static_cast<int>(intersect->y < 0.0 ? pos.y - 1.0 : pos.y), static_cast<int>(intersect->z < 0.0 ? pos.z - 1.0 : pos.z) };
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

		chunkNegY->clearMesh();
		m_Manager.getAdjacentChunks(chunkNegY->getLocation(), adjacentChunks);
		for (int i{}; i < 16; ++i)
		{
			chunkNegY->buildMesh(m_Manager, i, adjacentChunks);
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
			blockPos.x -= 1;

		if (currentPos.y < 0.0)
			blockPos.y -= 1;

		if (currentPos.z < 0.0)
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

glm::dvec3* Player::placeIntersect()
{
	glm::dvec3 camFront{ m_Cam.getFront() * vecPrecision };
	glm::dvec3 currentPos{ m_Cam.getLocation() };
	glm::dvec3* placePos{ nullptr };

	while (glm::length((m_Cam.getLocation() - currentPos)) < m_Reach)
	{
		Vector3i blockPos{ static_cast<int>(currentPos.x), static_cast<int>(currentPos.y), static_cast<int>(currentPos.z) };
		if (currentPos.x < 0.0)
			blockPos.x -= 1;

		if (currentPos.y < 0.0)
			blockPos.y -= 1;

		if (currentPos.z < 0.0)
			blockPos.z -= 1;

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