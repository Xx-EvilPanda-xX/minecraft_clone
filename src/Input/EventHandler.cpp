#include <glad/glad.h>
#include "EventHandler.h"
#include "../Constants.h"

BlockType EventHandler::selectedBlock{ BlockType::Stone };

void EventHandler::keyboardEvent(Keyboard& keyboard, Application& app, Player& player)
{
	static float flyToggleCooldown{};
	static float spectateToggleCooldown{};

	if (keyboard.isKeyDown(GLFW_KEY_ESCAPE))
		glfwSetWindowShouldClose(app.getWindow().getWindow(), true);

	if (keyboard.isKeyDown(GLFW_KEY_LEFT_CONTROL))
		player.setSprinting(true);
	else
		player.setSprinting(false);

	float velocityLimit{ player.isSprinting() ? 15.0f : 3.5f };

	if (keyboard.isKeyDown(GLFW_KEY_W))
	{
		if (player.getVelocity().z < velocityLimit)
			player.getVelocity().z += (constants::playerDrift * 10.0f) * Application::s_Dt;
	}
		
	if (keyboard.isKeyDown(GLFW_KEY_A))
	{
		if (player.getVelocity().x > -velocityLimit)
			player.getVelocity().x -= (constants::playerDrift * 10.0f) * Application::s_Dt;
	}
		
	if (keyboard.isKeyDown(GLFW_KEY_S))
	{
		if (player.getVelocity().z > -velocityLimit)
			player.getVelocity().z -= (constants::playerDrift * 10.0f) * Application::s_Dt;
	}
		
	if (keyboard.isKeyDown(GLFW_KEY_D))
	{
		if (player.getVelocity().x < velocityLimit)
			player.getVelocity().x += (constants::playerDrift * 10.0f) * Application::s_Dt;
	}

	if (keyboard.isKeyDown(GLFW_KEY_SPACE))
	{
		if (player.isFlying())
		{
			if (player.getVelocity().y < velocityLimit)
				player.getVelocity().y += (constants::playerDrift * 10.0f) * Application::s_Dt;
		}
		else if (player.isGrounded())
		{
			player.getVelocity().y += constants::jumpHeight * 10.0f;
		}
	}

	if (keyboard.isKeyDown(GLFW_KEY_LEFT_SHIFT))
	{
		if (player.isFlying())
		{
			if (player.getVelocity().y > -velocityLimit)
				player.getVelocity().y -= (constants::playerDrift * 10.0f) * Application::s_Dt;
		}
	}

	if (keyboard.isKeyDown(GLFW_KEY_C))
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	if (keyboard.isKeyDown(GLFW_KEY_F) && flyToggleCooldown <= 0.0f)
	{
		player.setFlying(!player.isFlying());
		flyToggleCooldown = 0.15f;
	}

	flyToggleCooldown -= Application::s_Dt;
	
	if (keyboard.isKeyDown(GLFW_KEY_N) && spectateToggleCooldown <= 0.0f)
	{
		player.setSpectating(!player.isSpectating());
		spectateToggleCooldown = 0.15f;
	}
	
	spectateToggleCooldown -= Application::s_Dt;

	if (keyboard.isKeyDown(GLFW_KEY_B))
		std::cout << "XYZ: " << app.getCamera().getLocation().x << ", " << app.getCamera().getLocation().y << ", " << app.getCamera().getLocation().z << "\n";

	if (keyboard.isKeyDown(GLFW_KEY_O))
		app.getWorld().reloadChunks(app.getCamera());

	if (keyboard.isKeyDown(GLFW_KEY_V))
	{
		Vector2i chunkPos{ static_cast<int>(app.getCamera().getLocation().x) / 16, static_cast<int>(app.getCamera().getLocation().z) / 16 };
		std::cout << "Chunk index: " << app.getWorld().getChunkIndex(chunkPos) << "\n";
	}

	if (keyboard.isKeyDown(GLFW_KEY_1))
		selectedBlock = BlockType::Grass;
	if (keyboard.isKeyDown(GLFW_KEY_2))
		selectedBlock = BlockType::Stone;
	if (keyboard.isKeyDown(GLFW_KEY_3))
		selectedBlock = BlockType::Dirt;
	if (keyboard.isKeyDown(GLFW_KEY_4))
		selectedBlock = BlockType::CobbleStone;
	if (keyboard.isKeyDown(GLFW_KEY_5))
		selectedBlock = BlockType::Wood;
	if (keyboard.isKeyDown(GLFW_KEY_6))
		selectedBlock = BlockType::Leaves;
	if (keyboard.isKeyDown(GLFW_KEY_7))
		selectedBlock = BlockType::Glass;
	if (keyboard.isKeyDown(GLFW_KEY_8))
		selectedBlock = BlockType::Sand;
	if (keyboard.isKeyDown(GLFW_KEY_9))
		selectedBlock = BlockType::Planks;
	if (keyboard.isKeyDown(GLFW_KEY_0))
		selectedBlock = BlockType::QotBu;

	if (keyboard.isKeyDown(GLFW_KEY_R))
		std::cout << "Address of first chunk: " << app.getWorld().getChunks()[0] << "\n";
}

void EventHandler::mouseEvent(Mouse& mouse, Player& player)
{
	static float breakCooldown{};
	static float placeCooldown{};

	if (mouse.isButtonDown(GLFW_MOUSE_BUTTON_LEFT) && breakCooldown <= 0.0f)
	{
		player.breakBlock();
		breakCooldown = constants::blockBreakCooldown;
	}

	if (mouse.isButtonDown(GLFW_MOUSE_BUTTON_RIGHT) && placeCooldown <= 0.0f)
	{
		player.placeBlock(selectedBlock);
		placeCooldown = constants::blockPlaceCooldown;
	}

	breakCooldown -= Application::s_Dt;
	placeCooldown -= Application::s_Dt;
}