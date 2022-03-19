#include <glad/glad.h>
#include "EventHandler.h"
#include "../Application.h"
#include "../Constants.h"

EventHandler::EventHandler()
	: m_SelectedBlock{ BlockType::Stone }
{
}

void EventHandler::handleKeyboard(Keyboard& keyboard, Application& app, Player& player)
{
	static double flyToggleCooldown{};

	if (keyboard.isKeyDown(GLFW_KEY_ESCAPE))
		glfwSetWindowShouldClose(app.getWindow().getWindow(), true);

	if (keyboard.isKeyDown(GLFW_KEY_LEFT_CONTROL))
		player.setSprinting(true);
	else
		player.setSprinting(false);

	double velocityLimit{ player.isSprinting() ? 15.0 : 3.5 };

	if (keyboard.isKeyDown(GLFW_KEY_W))
	{
		if (player.getVelocity().z < velocityLimit)
			player.getVelocity().z += (constants::playerDrift * 10.0) * Application::s_Dt;
	}
		
	if (keyboard.isKeyDown(GLFW_KEY_A))
	{
		if (player.getVelocity().x > -velocityLimit)
			player.getVelocity().x -= (constants::playerDrift * 10.0) * Application::s_Dt;
	}
		
	if (keyboard.isKeyDown(GLFW_KEY_S))
	{
		if (player.getVelocity().z > -velocityLimit)
			player.getVelocity().z -= (constants::playerDrift * 10.0) * Application::s_Dt;
	}
		
	if (keyboard.isKeyDown(GLFW_KEY_D))
	{
		if (player.getVelocity().x < velocityLimit)
			player.getVelocity().x += (constants::playerDrift * 10.0) * Application::s_Dt;
	}

	if (keyboard.isKeyDown(GLFW_KEY_SPACE))
	{
		if (player.isFlying())
		{
			if (player.getVelocity().y < velocityLimit)
				player.getVelocity().y += (constants::playerDrift * 10.0) * Application::s_Dt;
		}
		else if (player.isGrounded())
		{
			player.getVelocity().y += constants::jumpHeight * 10.0;
		}
	}

	if (keyboard.isKeyDown(GLFW_KEY_LEFT_SHIFT))
	{
		if (player.isFlying())
		{
			if (player.getVelocity().y > -velocityLimit)
				player.getVelocity().y -= (constants::playerDrift * 10.0) * Application::s_Dt;
		}
	}

	if (keyboard.isKeyDown(GLFW_KEY_C))
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	if (keyboard.isKeyDown(GLFW_KEY_F) && flyToggleCooldown <= 0.0)
	{
		player.setFlying(!player.isFlying());
		flyToggleCooldown = 0.15f;
	}

	flyToggleCooldown -= Application::s_Dt;

	if (keyboard.isKeyDown(GLFW_KEY_B))
		std::cout << "XYZ: " << app.getCamera().getLocation().x << ", " << app.getCamera().getLocation().y << ", " << app.getCamera().getLocation().z << "\n";

	if (keyboard.isKeyDown(GLFW_KEY_O))
		app.getWorld().reloadChunks(app.getCamera());

	if (keyboard.isKeyDown(GLFW_KEY_V))
	{
		Vector2i chunkPos{ static_cast<int>(app.getCamera().getLocation().x) / 16, static_cast<int>(app.getCamera().getLocation().z) / 16 };
		std::cout << "Chunk index: " << app.getWorld().getChunkIndex(chunkPos) << "\n";
	}

	if (keyboard.isKeyDown(GLFW_KEY_GRAVE_ACCENT))
		m_SelectedBlock = BlockType::Gravel;
	if (keyboard.isKeyDown(GLFW_KEY_1))
		m_SelectedBlock = BlockType::Grass;
	if (keyboard.isKeyDown(GLFW_KEY_2))
		m_SelectedBlock = BlockType::Stone;
	if (keyboard.isKeyDown(GLFW_KEY_3))
		m_SelectedBlock = BlockType::Dirt;
	if (keyboard.isKeyDown(GLFW_KEY_4))
		m_SelectedBlock = BlockType::CobbleStone;
	if (keyboard.isKeyDown(GLFW_KEY_5))
		m_SelectedBlock = BlockType::Wood;
	if (keyboard.isKeyDown(GLFW_KEY_6))
		m_SelectedBlock = BlockType::Leaves;
	if (keyboard.isKeyDown(GLFW_KEY_7))
		m_SelectedBlock = BlockType::Glass;
	if (keyboard.isKeyDown(GLFW_KEY_8))
		m_SelectedBlock = BlockType::Sand;
	if (keyboard.isKeyDown(GLFW_KEY_9))
		m_SelectedBlock = BlockType::Planks;
	if (keyboard.isKeyDown(GLFW_KEY_0))
		m_SelectedBlock = BlockType::DiamondBlock;
	if (keyboard.isKeyDown(GLFW_KEY_MINUS))
		m_SelectedBlock = BlockType::Water;
	if (keyboard.isKeyDown(GLFW_KEY_EQUAL))
		m_SelectedBlock = BlockType::CraftingTable;


	if (keyboard.isKeyDown(GLFW_KEY_R))
		std::cout << "Address of first chunk: " << app.getWorld().getChunks()[0] << "\n";
}

void EventHandler::handleMouse(Mouse& mouse, Player& player)
{
	static double breakCooldown{};
	static double placeCooldown{};

	if (mouse.isButtonDown(GLFW_MOUSE_BUTTON_LEFT) && breakCooldown <= 0.0)
	{
		player.breakBlock();
		breakCooldown = constants::blockBreakCooldown;
	}

	if (mouse.isButtonDown(GLFW_MOUSE_BUTTON_RIGHT) && placeCooldown <= 0.0)
	{
		player.placeBlock(m_SelectedBlock);
		placeCooldown = constants::blockPlaceCooldown;
	}

	breakCooldown -= Application::s_Dt;
	placeCooldown -= Application::s_Dt;
}