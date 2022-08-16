#include <glad/glad.h>
#include "EventHandler.h"
#include "../Application.h"
#include "../Constants.h"

EventHandler::EventHandler()
	: m_SelectedBlock{ BlockType::Stone, false },
	m_IsWireFrame{ false }
{
}

void EventHandler::handleKeyboard(Keyboard& keyboard, Application& app)
{
	static double flyToggleCooldown{};
	// for some fucking reason it crashes when there are too many consequtive
	// rebuilds but im sick and tired of debugging race conditions and memory 
	// bugs so imma just give it a cooldown. Come find me in the ocean with 
	// the rest of the rustaceans. f u cpp
	static double chunkRebuildCooldown{};
	Player& player{ app.getWorld().getPlayer() };
	glm::dvec3& v{ player.getVelocity() };
	glm::bvec3& decreasingV{ player.getDecreasingVel() };
	Camera& cam{ player.getCamera() };
	double velocityLimit{ player.isSprinting() ? constants::sprintSpeed : constants::walkSpeed };
	velocityLimit *= player.isFlying() ? 2.5 : 1.0;

	if (keyboard.isKeyDown(GLFW_KEY_LEFT_CONTROL))
		player.setSprinting(true);
	else
		player.setSprinting(false);

	if (keyboard.isKeyDown(GLFW_KEY_W) && !decreasingV.z)
	{
		if (v.z < velocityLimit)
			v.z += constants::playerDrift * 0.1;
		if (v.z > velocityLimit)
			v.z = velocityLimit;
	}
		
	if (keyboard.isKeyDown(GLFW_KEY_A) && !decreasingV.x)
	{
		if (v.x > -velocityLimit)
			player.getVelocity().x -= constants::playerDrift * 0.1;
		if (player.getVelocity().x < -velocityLimit)
			player.getVelocity().x = -velocityLimit;
	}
		
	if (keyboard.isKeyDown(GLFW_KEY_S) && !decreasingV.z)
	{
		if (v.z > -velocityLimit)
			v.z -= constants::playerDrift * 0.1;
		if (v.z < -velocityLimit)
			v.z = -velocityLimit;
	}
		
	if (keyboard.isKeyDown(GLFW_KEY_D) && !decreasingV.x)
	{
		if (v.x < velocityLimit)
			v.x += constants::playerDrift * 0.1;
		if (v.x > velocityLimit)
			v.x = velocityLimit;
	}

	if (keyboard.isKeyDown(GLFW_KEY_SPACE) && !decreasingV.y)
	{
		if (player.isFlying())
		{
			if (v.y < velocityLimit)
				v.y += constants::playerDrift * 0.1;
			if (v.y > velocityLimit)
				v.y = velocityLimit;
		}
		else if (player.isGrounded())
		{
			v.y += constants::jumpHeight * 10.0;
		}
	}

	if (keyboard.isKeyDown(GLFW_KEY_LEFT_SHIFT) && !decreasingV.y)
	{
		if (player.isFlying())
		{
			if (v.y > -velocityLimit)
				v.y -= constants::playerDrift * 0.1;
			if (v.y < -velocityLimit)
				v.y = -velocityLimit;
		}
	}

	if (keyboard.isKeyDown(GLFW_KEY_ESCAPE))
		glfwSetWindowShouldClose(app.getWindow().getGlfwWindow(), true);

	if (keyboard.isKeyDown(GLFW_KEY_C))
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		m_IsWireFrame = true;
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		m_IsWireFrame = false;
	}
		
	if (keyboard.isKeyDown(GLFW_KEY_F) && flyToggleCooldown <= 0.0)
	{
		player.setFlying(!player.isFlying());

		if (!player.isFlying())
			player.setHasTouchedGround(false);

		flyToggleCooldown = 0.15f;
	}

	if (keyboard.isKeyDown(GLFW_KEY_R) && chunkRebuildCooldown <= 0.0) {
		app.getWorld().rebuildChunks(cam);
		chunkRebuildCooldown = 1.0f;
	}

	if (keyboard.isKeyDown(GLFW_KEY_LEFT_ALT))
	{
		cam.setFov(20.0);
		cam.setMouseSens(constants::mouseSensitivity / 5.0);
	}
	else
	{
		cam.setFov(90.0);
		cam.setMouseSens(constants::mouseSensitivity);
	}

	if (keyboard.isKeyDown(GLFW_KEY_TAB))
	{
		if (keyboard.isKeyDown(GLFW_KEY_1))
			m_SelectedBlock = Block{ BlockType::Cactus, false };
		if (keyboard.isKeyDown(GLFW_KEY_2))
			m_SelectedBlock = Block{ BlockType::Gravel, false };
		if (keyboard.isKeyDown(GLFW_KEY_3))
			m_SelectedBlock = Block{ BlockType::DiamondBlock, false };
		if (keyboard.isKeyDown(GLFW_KEY_4))
			m_SelectedBlock = Block{ BlockType::Snow, false };
		if (keyboard.isKeyDown(GLFW_KEY_5))
			m_SelectedBlock = Block{ BlockType::Water, false };
		if (keyboard.isKeyDown(GLFW_KEY_6))
			m_SelectedBlock = Block{ BlockType::CraftingTable, false };
		if (keyboard.isKeyDown(GLFW_KEY_7))
			m_SelectedBlock = Block{ BlockType::Flowers, false };
		if (keyboard.isKeyDown(GLFW_KEY_8))
			m_SelectedBlock = Block{ BlockType::TallGrass, false };
		if (keyboard.isKeyDown(GLFW_KEY_9))
			m_SelectedBlock = Block{ BlockType::Shrub, false };
		if (keyboard.isKeyDown(GLFW_KEY_0))
			m_SelectedBlock = Block{ BlockType::Rose, false };
	}
	else if (keyboard.isKeyDown(GLFW_KEY_CAPS_LOCK))
	{
		if (keyboard.isKeyDown(GLFW_KEY_1))
			m_SelectedBlock = Block{ BlockType::RedWool, false };
		if (keyboard.isKeyDown(GLFW_KEY_2))
			m_SelectedBlock = Block{ BlockType::OrangeWool, false };
		if (keyboard.isKeyDown(GLFW_KEY_3))
			m_SelectedBlock = Block{ BlockType::YellowWool, false };
		if (keyboard.isKeyDown(GLFW_KEY_4))
			m_SelectedBlock = Block{ BlockType::GreenWool, false };
		if (keyboard.isKeyDown(GLFW_KEY_5))
			m_SelectedBlock = Block{ BlockType::SkyBlueWool, false };
		if (keyboard.isKeyDown(GLFW_KEY_6))
			m_SelectedBlock = Block{ BlockType::BlueWool, false };
		if (keyboard.isKeyDown(GLFW_KEY_7))
			m_SelectedBlock = Block{ BlockType::PurpleWool, false };
		if (keyboard.isKeyDown(GLFW_KEY_8))
			m_SelectedBlock = Block{ BlockType::PinkWool, false };
		if (keyboard.isKeyDown(GLFW_KEY_9))
			m_SelectedBlock = Block{ BlockType::BrownWool, false };
		if (keyboard.isKeyDown(GLFW_KEY_0))
			m_SelectedBlock = Block{ BlockType::BlackWool, false };
	}
	else
	{
		if (keyboard.isKeyDown(GLFW_KEY_1))
			m_SelectedBlock = Block{ BlockType::Grass, false };
		if (keyboard.isKeyDown(GLFW_KEY_2))
			m_SelectedBlock = Block{ BlockType::Stone, false };
		if (keyboard.isKeyDown(GLFW_KEY_3))
			m_SelectedBlock = Block{ BlockType::Dirt, false };
		if (keyboard.isKeyDown(GLFW_KEY_4))
			m_SelectedBlock = Block{ BlockType::CobbleStone, false };
		if (keyboard.isKeyDown(GLFW_KEY_5))
			m_SelectedBlock = Block{ BlockType::Wood, false };
		if (keyboard.isKeyDown(GLFW_KEY_6))
			m_SelectedBlock = Block{ BlockType::PalmLeaves, false };
		if (keyboard.isKeyDown(GLFW_KEY_7))
			m_SelectedBlock = Block{ BlockType::OakLeaves, false };
		if (keyboard.isKeyDown(GLFW_KEY_8))
			m_SelectedBlock = Block{ BlockType::Glass, false };
		if (keyboard.isKeyDown(GLFW_KEY_9))
			m_SelectedBlock = Block{ BlockType::Sand, false };
		if (keyboard.isKeyDown(GLFW_KEY_0))
			m_SelectedBlock = Block{ BlockType::Planks, false };
	}

	flyToggleCooldown -= Application::s_Dt;
	chunkRebuildCooldown -= Application::s_Dt;
}

void EventHandler::handleMouse(Mouse& mouse, Player& player)
{
	static double breakCooldown{};
	static double placeCooldown{};

	if (!mouse.isButtonDown(GLFW_MOUSE_BUTTON_LEFT))
		breakCooldown = 0.0;

	if (!mouse.isButtonDown(GLFW_MOUSE_BUTTON_RIGHT))
		placeCooldown = 0.0;

	if (mouse.isButtonDown(GLFW_MOUSE_BUTTON_LEFT) && breakCooldown <= 0.0)
	{
		player.breakBlock();
		breakCooldown = constants::blockBreakCooldown;
	}

	if (mouse.isButtonDown(GLFW_MOUSE_BUTTON_RIGHT) && placeCooldown <= 0.0)
	{
		player.placeBlock(m_SelectedBlock.getType());
		placeCooldown = constants::blockPlaceCooldown;
	}

	breakCooldown -= Application::s_Dt;
	placeCooldown -= Application::s_Dt;
}

Block& EventHandler::getSelectedBlock()
{
	return m_SelectedBlock;
}

void EventHandler::setSelectedBlock(Block newBlock)
{
	m_SelectedBlock = newBlock;
}