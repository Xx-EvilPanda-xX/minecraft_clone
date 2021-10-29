#include <iostream>
#include "Application.h"

int main()
{
	Application app{};
	app.init(1280, 720, "Minecraft!");
	app.runMainLoop();
}

