#include "Player.h"
#include "../World/Chunk.h"
#include <glm/vec3.hpp>
#include "../Application.h"
#include "../Constants.h"
#include <cmath>

Player::Player(ChunkManager& manager, Keyboard& keyboard, double reach)
	: m_Camera{ { 0.0, 96.0, 0.0 }, 0.0, 0.0, constants::cameraFov, constants::mouseSensitivity },
	m_Manager{ manager },
	m_Keyboard{ keyboard },
	m_Reach{ reach },
	m_Velocity{},
	m_LastValidLoc{},
	m_LastValidAABB{},
	m_LastMoved{},
	m_DecreasingVel{}
{
	m_JumpCoolDown = 0.25;
	m_Sprinting = false;
	m_Grounded = false;
	m_Flying = false;
	m_HasTouchedGround = false;
	m_FirstFrame = false;
}

void Player::move()
{
	calculateVelocity();

	//gravity
	if (!m_Flying && !m_Grounded && m_FirstFrame)
		m_Velocity.y -= constants::gravity * Application::s_Dt;
	
	m_Camera.handleMove(m_Velocity, Application::s_Dt);
	m_Aabb = createPlayerAABB(m_Camera.getLocation());
	
	if (m_FirstFrame)
	{
		collisionDetection();
		m_LastMoved.x = m_LastValidLoc.x != m_Camera.getLocation().x;
		m_LastMoved.y = m_LastValidLoc.y != m_Camera.getLocation().y;
		m_LastMoved.z = m_LastValidLoc.z != m_Camera.getLocation().z;
	}

	m_JumpCoolDown -= Application::s_Dt;

	m_LastValidLoc = m_Camera.getLocation();
	m_LastValidAABB = m_Aabb;

	if (m_Camera.getLocation().y <= worldBottom)
	{
		m_Camera.setY(worldBottom);
		m_Velocity.y = 0.0;
		m_Grounded = true;
	}

	m_FirstFrame = true;
}

void Player::collisionDetection()
{
	glm::dvec3 lowerPlayerHalf{ m_Camera.getLocation().x,
								m_Aabb.min().y + constants::playerSize,
								m_Camera.getLocation().z };

	glm::dvec3 upperPlayerHalf{ m_Camera.getLocation().x,
								m_Aabb.max().y - constants::playerSize,
								m_Camera.getLocation().z };

	bool onGround{ false };
	CollisionType collisionType{};
	Vector3i collisionPos{};

	//3 iters, one per axis
	for (int i{}; i < 3; ++i)
	{
		if (collide(lowerPlayerHalf, upperPlayerHalf, m_Aabb, collisionPos, collisionType))
		{
			glm::dvec3 blockCenter{ collisionPos.x + 0.5, collisionPos.y + 0.5, collisionPos.z + 0.5 };
			glm::dvec3 lastValidLoc;

			switch (collisionType)
			{
			case CollisionType::PlayerUpperHalf:
				lastValidLoc = { m_LastValidLoc.x,
								m_LastValidAABB.max().y - constants::playerSize,
								m_LastValidLoc.z };
				break;
			case CollisionType::PlayerLowerHalf:
				lastValidLoc = { m_LastValidLoc.x,
								m_LastValidAABB.min().y + constants::playerSize,
								m_LastValidLoc.z };
				break;
			}

			glm::dvec3 direction{ glm::normalize(lastValidLoc - blockCenter) * vecPrecision };
			glm::dvec3 testLoc{ blockCenter };
			Vector3i block{ testLoc };
			while (block == collisionPos)
			{
				testLoc += direction;
				block = Vector3i{ testLoc };
			}

			bool collideX{ block.x != collisionPos.x };
			bool collideY{ block.y != collisionPos.y };
			bool collideZ{ block.z != collisionPos.z };
			int intersects{};

			if (collideX) ++intersects;
			if (collideY) ++intersects;
			if (collideZ) ++intersects;

			if (intersects != 1)
			{
				collideX = false;
				collideY = false;
				collideZ = false;

				if (!m_LastMoved.x && m_LastMoved.y && m_LastMoved.z)
				{
					if (blockCenter.y < m_Aabb.min().y || blockCenter.y > m_Aabb.max().y)
						collideY = true;
					else
						collideX = true;
				}


				if (m_LastMoved.x && !m_LastMoved.y && m_LastMoved.z)
				{
					if (blockCenter.y > m_Aabb.min().y && blockCenter.y < m_Aabb.max().y)
						collideX = true;
					else
						collideY = true;
				}

				if (m_LastMoved.x && m_LastMoved.y && !m_LastMoved.z)
				{
					if (blockCenter.y < m_Aabb.min().y || blockCenter.y > m_Aabb.max().y)
						collideY = true;
					else
						collideZ = true;
				}

				if (!m_LastMoved.x && !m_LastMoved.y && m_LastMoved.z)
				{
					if (blockCenter.y > m_Aabb.min().y && blockCenter.y < m_Aabb.max().y)
						collideX = true;
					else
						collideY = true;
				}


				if (m_LastMoved.x && !m_LastMoved.y && !m_LastMoved.z)
				{
					if (blockCenter.y > m_Aabb.min().y && blockCenter.y < m_Aabb.max().y)
						collideZ = true;
					else
						collideY = true;
				}

				if (!m_LastMoved.x && m_LastMoved.y && !m_LastMoved.z)
				{
					if (blockCenter.y > m_Aabb.min().y && blockCenter.y < m_Aabb.max().y)
						collideX = true;
					else
						collideY = true;
				}

				if (m_LastMoved.x && m_LastMoved.y && m_LastMoved.z)
				{
					if (blockCenter.y > m_Aabb.min().y && blockCenter.y < m_Aabb.max().y)
						collideX = true;
					else
						collideY = true;
				}
			}

			const double targetDistance{ 0.5 + constants::playerSize + 0.0000001 };

			if (collideX)
				m_Camera.setX(blockCenter.x < lastValidLoc.x ?
							(blockCenter.x + targetDistance) :
							(blockCenter.x - targetDistance));

			if (collideY)
			{
				m_Camera.setY(blockCenter.y < lastValidLoc.y ?
							(blockCenter.y + targetDistance) + (constants::playerSize * 2.0) + cameraHeightDiff :
							(blockCenter.y - targetDistance) + cameraHeightDiff);

				m_Velocity.y = 0.0;

				if (collisionType == CollisionType::PlayerLowerHalf && m_JumpCoolDown <= 0.0)
				{
					onGround = true;
					m_HasTouchedGround = true;
					if (m_Keyboard.isKeyDown(GLFW_KEY_SPACE))
						m_JumpCoolDown = 0.025;
				}
			}

			if (collideZ)
				m_Camera.setZ(blockCenter.z < lastValidLoc.z ?
							(blockCenter.z + targetDistance) :
							(blockCenter.z - targetDistance));

			m_Aabb = createPlayerAABB(m_Camera.getLocation());

			lowerPlayerHalf = { m_Camera.getLocation().x,
								m_Aabb.min().y + constants::playerSize,
								m_Camera.getLocation().z };

			upperPlayerHalf = { m_Camera.getLocation().x,
								m_Aabb.max().y - constants::playerSize,
								m_Camera.getLocation().z };
		}
	}

	m_Grounded = onGround;
}

AABB Player::createPlayerAABB(glm::dvec3 playerPos)
{
	AABB aabb{};
	// - 0.35f to make player's head appear higher
	aabb.min(glm::dvec3{ playerPos.x - constants::playerSize,
						playerPos.y - (3.0 * constants::playerSize) - cameraHeightDiff,
						playerPos.z - constants::playerSize });

	aabb.max(glm::dvec3{ playerPos.x + constants::playerSize,
						playerPos.y + constants::playerSize - cameraHeightDiff,
						playerPos.z + constants::playerSize });
	return aabb;
}

bool Player::collide(glm::dvec3 playerLowerHalf, glm::dvec3 playerUpperHalf,
					const AABB& playerAABB, Vector3i& o_Pos,
					CollisionType& o_CollisionType)
{
	double lowerClosestCollision{};
	Vector3i* lowerCollisionPos{ test(playerLowerHalf, playerAABB, lowerClosestCollision) };

	double upperClosestCollision{};
	Vector3i* upperCollisionPos{ test(playerUpperHalf, playerAABB, upperClosestCollision) };

	bool collision{};

	if (lowerCollisionPos && upperCollisionPos)
	{
		if (lowerClosestCollision <= upperClosestCollision)
		{
			o_Pos = *lowerCollisionPos;
			o_CollisionType = CollisionType::PlayerLowerHalf;
		}
		if (upperClosestCollision <= lowerClosestCollision)
		{
			o_Pos = *upperCollisionPos;
			o_CollisionType = CollisionType::PlayerUpperHalf;
		}
		collision = true;
	}

	if (lowerCollisionPos) delete lowerCollisionPos;
	if (upperCollisionPos) delete upperCollisionPos;
	return collision;
}

Vector3i* Player::test(glm::dvec3 playerPos, const AABB& playerAABB, double& o_ClosestCollision)
{
	Vector3i playerBlockPos{ playerPos };
	bool collision{ false };
	double closestCollision{ -1.0 };
	Vector3i* collisionPos{ nullptr };

	for (int x{ playerBlockPos.x - collisionSearchRadiusX }; x < playerBlockPos.x + collisionSearchRadiusX; ++x)
	{
		for (int y{ playerBlockPos.y - collisionSearchRadiusY }; y < playerBlockPos.y + collisionSearchRadiusY; ++y)
		{
			for (int z{ playerBlockPos.z - collisionSearchRadiusZ }; z < playerBlockPos.z + collisionSearchRadiusZ; ++z)
			{
				AABB blockAABB{ glm::dvec3{ x, y, z }, glm::dvec3{ x + 1, y + 1, z + 1 } };
				Block block{ m_Manager.getWorldBlock(Vector3i{ x, y, z }) };
				if (blockAABB.intersects(playerAABB) && 
					(block.getType() != BlockType::Air &&
					block.getType() != BlockType::Water &&
					block.getType() != BlockType::TallGrass &&
					block.getType() != BlockType::Shrub && 
					block.getType() != BlockType::Rose))
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
	const bool ctrl{ m_Keyboard.isKeyDown(GLFW_KEY_LEFT_CONTROL) };
	const bool a{ m_Keyboard.isKeyDown(GLFW_KEY_A) };
	const bool s{ m_Keyboard.isKeyDown(GLFW_KEY_S) };
	const bool w{ m_Keyboard.isKeyDown(GLFW_KEY_W) };
	const bool d{ m_Keyboard.isKeyDown(GLFW_KEY_D) };
	const bool shift{ m_Keyboard.isKeyDown(GLFW_KEY_LEFT_SHIFT) };
	const bool space{ m_Keyboard.isKeyDown(GLFW_KEY_SPACE) };

	const double speed{ m_Flying ? (m_Sprinting ? constants::sprintSpeed : constants::walkSpeed) * 2.5 : (m_Sprinting ? constants::sprintSpeed : constants::walkSpeed) };
	const double drift{ m_Flying ? constants::playerDrift * 2.0 : constants::playerDrift };

	if (m_Velocity.x < speed && m_Velocity.x > -speed && m_DecreasingVel.x && !m_HasTouchedGround)
		m_HasTouchedGround = true;
	if (m_Velocity.z < speed && m_Velocity.z > -speed && m_DecreasingVel.z && !m_HasTouchedGround)
		m_HasTouchedGround = true;

	m_DecreasingVel.x = (m_Velocity.x > speed || m_Velocity.x < -speed) && (((a || d) && !ctrl) || (!m_Flying && !m_HasTouchedGround));
	m_DecreasingVel.y = (m_Velocity.y > speed || m_Velocity.y < -speed) && ((shift || space) && m_Flying) && !ctrl;
	m_DecreasingVel.z = (m_Velocity.z > speed || m_Velocity.z < -speed) && (((w || s) && !ctrl) || (!m_Flying && !m_HasTouchedGround));

	//std::cout << "DecreasingVel: " << m_DecreasingVel.x << ", " << m_DecreasingVel.y << ", " << m_DecreasingVel.z << "\t\t";
	//std::cout << "Velocity: " << m_Velocity.x << ", " << m_Velocity.y << ", " << m_Velocity.z << ", " << speed << "\n";

	if (!a || m_DecreasingVel.x)
	{
		if (m_Velocity.x < 0.0)
		{
			m_Velocity.x += drift * Application::s_Dt;
			if (m_Velocity.x > 0.0) m_Velocity.x = 0.0;
		}
	}

	if (!d || m_DecreasingVel.x)
	{
		if (m_Velocity.x > 0.0)
		{
			m_Velocity.x -= drift * Application::s_Dt;
			if (m_Velocity.x < 0.0) m_Velocity.x = 0.0;
		}
	}

	if (!shift || m_DecreasingVel.y)
	{
		if (m_Velocity.y < 0.0)
		{
			m_Velocity.y += drift * Application::s_Dt;
			if (m_Velocity.y > 0.0) m_Velocity.y = 0.0;
		}
	}

	if ((!space && m_Flying) || m_DecreasingVel.y)
	{
		if (m_Velocity.y > 0.0)
		{
			m_Velocity.y -= drift * Application::s_Dt;
			if (m_Velocity.y < 0.0) m_Velocity.y = 0.0;
		}
	}

	if (!s || m_DecreasingVel.z)
	{
		if (m_Velocity.z < 0.0)
		{
			m_Velocity.z += drift * Application::s_Dt;
			if (m_Velocity.z > 0.0) m_Velocity.z = 0.0;
		}
	}

	if (!w || m_DecreasingVel.z)
	{
		if (m_Velocity.z > 0.0)
		{
			m_Velocity.z -= drift * Application::s_Dt;
			if (m_Velocity.z < 0.0) m_Velocity.z = 0.0;
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
	Vector3i* breakPos{ intersect<Vector3i>(false) };

	if (breakPos != nullptr)
	{
		Vector3i pos{ *breakPos };

		if (!m_Manager.chunkExsists(pos))
			return;

		m_Manager.setWorldBlock(pos, BlockType::Air, false, true);

		updateMeshes(pos);

		delete breakPos;
	}
}

void Player::placeBlock(BlockType type)
{
	glm::dvec3 camFront{ m_Camera.getFront() * vecPrecision };
	glm::dvec3* inter{ intersect<glm::dvec3>(type != BlockType::TallGrass && type != BlockType::Rose && type != BlockType::Shrub) };

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

		AABB blockAABB{ glm::dvec3{ placePos.x, placePos.y, placePos.z },
						glm::dvec3{ placePos.x + 1, placePos.y + 1, placePos.z + 1 } };

		if (m_Aabb.intersects(blockAABB))
			return;

		bool surface{ m_Manager.getWorldBlock({placePos.x, placePos.y + 1, placePos.z}).getType() == BlockType::Air && type == BlockType::Water };
		m_Manager.setWorldBlock(placePos, type, surface, true);

		updateMeshes(placePos);

		delete inter;
	}
}

template <typename T>
T* Player::intersect(bool throughFoliage)
{
	glm::dvec3 camFront{ m_Camera.getFront() * vecPrecision };
	glm::dvec3 currentPos{ m_Camera.getLocation() };
	T* intersectPos{ nullptr };

	while (glm::length((m_Camera.getLocation() - currentPos)) < m_Reach)
	{
		Vector3i blockPos{ currentPos };
		Block block{ m_Manager.getWorldBlock(blockPos) };
		if (block.getType() != BlockType::Air &&
			block.getType() != BlockType::Water && 
			(block.getType() != BlockType::TallGrass || !throughFoliage) &&
			(block.getType() != BlockType::Rose || !throughFoliage) &&
			(block.getType() != BlockType::Shrub || !throughFoliage))
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

	buildUpdatedMesh(chunk);

	Vector3i local{ m_Manager.toSectionLocal(editPos) };

	if (local.x == 15)
	{
		Chunk* chunkPosX{ m_Manager.getChunk(Vector2i{ chunk->getLocation().x + 1, chunk->getLocation().y }) };

		if (!chunkPosX->isBuilt())
			return;

		buildUpdatedMesh(chunkPosX);
	}

	if (local.x == 0)
	{
		Chunk* chunkNegX{ m_Manager.getChunk(Vector2i{ chunk->getLocation().x - 1, chunk->getLocation().y }) };
		
		if (!chunkNegX->isBuilt())
			return;

		buildUpdatedMesh(chunkNegX);
	}

	if (local.z == 15)
	{
		Chunk* chunkPosY{ m_Manager.getChunk(Vector2i{ chunk->getLocation().x, chunk->getLocation().y + 1 }) };
		
		if (!chunkPosY->isBuilt())
			return;
		
		buildUpdatedMesh(chunkPosY);
	}

	if (local.z == 0)
	{
		Chunk* chunkNegY{ m_Manager.getChunk(Vector2i{ chunk->getLocation().x, chunk->getLocation().y - 1 }) };
		
		if (!chunkNegY->isBuilt())
			return;
		
		buildUpdatedMesh(chunkNegY);
	}
}

void Player::buildUpdatedMesh(Chunk* chunk)
{
	int chunkIndex;

	if (m_Manager.isInBuildQueue(chunk, chunkIndex))
	{
		std::lock_guard<std::mutex> lock{ m_Manager.getBuildQueueMutex() };
		m_Manager.getBuildQueue().erase(m_Manager.getBuildQueue().begin() + chunkIndex);
	}

	chunk->clearMesh();
	Chunk* adjacentChunks[4]{};
	m_Manager.getAdjacentChunks(chunk->getLocation(), adjacentChunks);
	for (int i{}; i < 16; ++i)
	{
		chunk->buildMesh(m_Manager, i, adjacentChunks);
	}
	chunk->finishBuilding();
}

double Player::getReach() const
{
	return m_Reach;
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

bool Player::isHasTouchedGround() const
{
	return m_HasTouchedGround;
}

glm::bvec3& Player::getDecreasingVel()
{
	return m_DecreasingVel;
}

void Player::setReach(double reach)
{
	m_Reach = reach;
}

void Player::setSprinting(bool sprinting)
{
	m_Sprinting = sprinting;
}

void Player::setFlying(bool flying)
{
	m_Flying = flying;
}

void Player::setHasTouchedGround(bool hasTouchedGround)
{
	m_HasTouchedGround = hasTouchedGround;
}