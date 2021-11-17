#include <glad/glad.h>
#include "EventHandler.h"

namespace EventHandler
{
	void keyBoardEvent(Keyboard& keyboard, Application& app)
	{
		if (keyboard.isKeyDown(GLFW_KEY_ESCAPE))
			glfwSetWindowShouldClose(app.getWindow().getWindow(), true);

		if (keyboard.isKeyDown(GLFW_KEY_W))
		{
			app.getCamera().handleKeyboard(Direction::Forward, 5.0f, app.m_Dt);

			if (keyboard.isKeyDown(GLFW_KEY_LEFT_CONTROL))
				app.getCamera().handleKeyboard(Direction::Forward, 20.0f, app.m_Dt);
		}

		if (keyboard.isKeyDown(GLFW_KEY_A))
			app.getCamera().handleKeyboard(Direction::Left, 5.0f, app.m_Dt);

		if (keyboard.isKeyDown(GLFW_KEY_S))
			app.getCamera().handleKeyboard(Direction::Back, 5.0f, app.m_Dt);

		if (keyboard.isKeyDown(GLFW_KEY_D))
			app.getCamera().handleKeyboard(Direction::Right, 5.0f, app.m_Dt);

		if (keyboard.isKeyDown(GLFW_KEY_SPACE))
		{
			app.getCamera().handleKeyboard(Direction::Up, 5.0f, app.m_Dt);

			if (keyboard.isKeyDown(GLFW_KEY_LEFT_CONTROL))
				app.getCamera().handleKeyboard(Direction::Up, 20.0f, app.m_Dt);
		}

		if (keyboard.isKeyDown(GLFW_KEY_LEFT_SHIFT))
		{
			app.getCamera().handleKeyboard(Direction::Down, 5.0f, app.m_Dt);

			if (keyboard.isKeyDown(GLFW_KEY_LEFT_CONTROL))
				app.getCamera().handleKeyboard(Direction::Down, 20.0f, app.m_Dt);
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
	}
}