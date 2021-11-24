#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

#include "Keyboard.h"
#include "../Application.h"
#include "../Player/Player.h"

class EventHandler
{
private:
	static BlockType selectedBlock;

public:
	static void keyboardEvent(Keyboard& keyboard, Application& app, Player& player);

	static void mouseEvent(Mouse& mouse, Player& player);
};

#endif