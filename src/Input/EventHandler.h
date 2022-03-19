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
	BlockType m_SelectedBlock;

public:
	EventHandler();

	void handleKeyboard(Keyboard& keyboard, Application& app, Player& player);

	void handleMouse(Mouse& mouse, Player& player);
};

#endif