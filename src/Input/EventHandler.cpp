#include <glad/glad.h>
#include "EventHandler.h"
#include "../Application.h"
#include "../Constants.h"

EventHandler::EventHandler()
	: m_SelectedBlock{ BlockType::Stone, false },
	IsWireFrame{ false }
{
}

void EventHandler::handleKeyboard(Keyboard& keyboard, Application& app)
{
	static double flyToggleCooldown{};
	Player& player{ app.getWorld().getPlayer() };
	Camera& cam{ player.getCamera() };
	

	if (keyboard.isKeyDown(GLFW_KEY_ESCAPE))
		glfwSetWindowShouldClose(app.getWindow().getGlfwWindow(), true);

	if (keyboard.isKeyDown(GLFW_KEY_LEFT_CONTROL))
		player.setSprinting(true);
	else
		player.setSprinting(false);

	double velocityLimit{ player.isSprinting() ? constants::sprintSpeed : constants::walkSpeed };

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
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		IsWireFrame = true;
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		IsWireFrame = false;
	}
		
	if (keyboard.isKeyDown(GLFW_KEY_F) && flyToggleCooldown <= 0.0)
	{
		player.setFlying(!player.isFlying());
		flyToggleCooldown = 0.15f;
	}

	if (keyboard.isKeyDown(GLFW_KEY_O))
		app.getWorld().rebuildChunks(cam);

	if (keyboard.isKeyDown(GLFW_KEY_LEFT_ALT))
	{
		cam.setFov(20.0);
		cam.setMouseSens(constants::mouse_sensitivity / 5.0);
	}
	else
	{
		cam.setFov(90.0);
		cam.setMouseSens(constants::mouse_sensitivity);
	}

	if (keyboard.isKeyDown(GLFW_KEY_GRAVE_ACCENT))
		m_SelectedBlock = Block{ BlockType::Gravel, false };
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
	if (keyboard.isKeyDown(GLFW_KEY_MINUS))
		m_SelectedBlock = Block{ BlockType::DiamondBlock, false };
	if (keyboard.isKeyDown(GLFW_KEY_EQUAL))
		m_SelectedBlock = Block{ BlockType::Water, false };

	flyToggleCooldown -= Application::s_Dt;
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