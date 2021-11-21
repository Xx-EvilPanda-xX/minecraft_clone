#include <glad/glad.h>
#include "EventHandler.h"
#include "../Constants.h"

namespace EventHandler
{
	void keyboardEvent(Keyboard& keyboard, Application& app)
	{
		if (keyboard.isKeyDown(GLFW_KEY_ESCAPE))
			glfwSetWindowShouldClose(app.getWindow().getWindow(), true);

		if (keyboard.isKeyDown(GLFW_KEY_W))
		{
			app.getCamera().handleKeyboard(Direction::Forward, 5.0f, Application::m_Dt);

			if (keyboard.isKeyDown(GLFW_KEY_LEFT_CONTROL))
				app.getCamera().handleKeyboard(Direction::Forward, 20.0f, Application::m_Dt);
		}

		if (keyboard.isKeyDown(GLFW_KEY_A))
			app.getCamera().handleKeyboard(Direction::Left, 5.0f, Application::m_Dt);

		if (keyboard.isKeyDown(GLFW_KEY_S))
			app.getCamera().handleKeyboard(Direction::Back, 5.0f, Application::m_Dt);

		if (keyboard.isKeyDown(GLFW_KEY_D))
			app.getCamera().handleKeyboard(Direction::Right, 5.0f, Application::m_Dt);

		if (keyboard.isKeyDown(GLFW_KEY_SPACE))
		{
			app.getCamera().handleKeyboard(Direction::Up, 5.0f, Application::m_Dt);

			if (keyboard.isKeyDown(GLFW_KEY_LEFT_CONTROL))
				app.getCamera().handleKeyboard(Direction::Up, 20.0f, Application::m_Dt);
		}

		if (keyboard.isKeyDown(GLFW_KEY_LEFT_SHIFT))
		{
			app.getCamera().handleKeyboard(Direction::Down, 5.0f, Application::m_Dt);

			if (keyboard.isKeyDown(GLFW_KEY_LEFT_CONTROL))
				app.getCamera().handleKeyboard(Direction::Down, 20.0f, Application::m_Dt);
		}

		if (keyboard.isKeyDown(GLFW_KEY_C))
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		if (keyboard.isKeyDown(GLFW_KEY_F))
			std::cout << "XYZ: " << app.getCamera().getLocation().x << ", " << app.getCamera().getLocation().y << ", " << app.getCamera().getLocation().z << "\n";

		if (keyboard.isKeyDown(GLFW_KEY_G))
			app.getWorld()->reloadChunks(app.getCamera());

		if (keyboard.isKeyDown(GLFW_KEY_V))
		{
			Vector2i chunkPos{ static_cast<int>(app.getCamera().getLocation().x) / 16, static_cast<int>(app.getCamera().getLocation().z) / 16 };
			std::cout << "Chunk index: " << app.getWorld()->getChunkIndex(chunkPos) << "\n";
		}

		if (keyboard.isKeyDown(GLFW_KEY_R))
		{
			const char* blockName{};
			Vector3i blockPos{ static_cast<int>(app.getCamera().getLocation().x), static_cast<int>(app.getCamera().getLocation().y), static_cast<int>(app.getCamera().getLocation().z) };

			if (app.getCamera().getLocation().y < 0.0f)
				blockPos.y -= 1;

			Block block{ app.getWorld()->getManager().getWorldBlock(blockPos) };

			switch (block.getType())
			{
			case BlockType::Air:
				blockName = "Air";
				break;

			case BlockType::Dirt:
				blockName = "Dirt";
				break;

			case BlockType::Grass:
				blockName = "Grass";
				break;

			case BlockType::Stone:
				blockName = "Stone";
				break;

			default:
				blockName = "Unknown";
				break;
			}

			std::cout << "Block at player position: " << blockName << "\n";
		}
	}

	void mouseEvent(Mouse& mouse, Player& player)
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
			player.placeBlock(Block{ BlockType::Stone });
			placeCooldown = constants::blockPlaceCooldown;
		}

		breakCooldown -= Application::m_Dt;
		placeCooldown -= Application::m_Dt;
	}
}