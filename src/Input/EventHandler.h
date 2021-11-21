#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

#include "Keyboard.h"
#include "../Application.h"
#include "../Player/Player.h"

class EventHandler
{
private:
	static Block selectedBlock;

public:
	static void keyboardEvent(Keyboard& keyboard, Application& app);

	static void mouseEvent(Mouse& mouse, Player& player);
};

#endif