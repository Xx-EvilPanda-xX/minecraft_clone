#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

#include "Keyboard.h"
#include "../Application.h"
#include "../Player/Player.h"

namespace EventHandler
{
	void keyboardEvent(Keyboard& keyboard, Application& app);

	void mouseEvent(Mouse& mouse, Player& player);
}

#endif