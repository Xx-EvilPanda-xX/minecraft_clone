#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

#include "Keyboard.h"
#include "../Player/Player.h"
#include "Mouse.h"
#include "Keyboard.h"
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Application;

class EventHandler
{
private:
	Block m_SelectedBlock;

public:
	bool m_IsWireFrame;

	EventHandler();

	void handleKeyboard(Keyboard& keyboard, Application& app);

	void handleMouse(Mouse& mouse, Player& player);

	Block& getSelectedBlock();

	void setSelectedBlock(Block newBlock);
};

#endif