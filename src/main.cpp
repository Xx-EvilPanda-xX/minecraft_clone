#include <iostream>
#include "Application.h"
#include "Constants.h"

int main()
{
	Application app{};
	app.init(constants::windowWidth, constants::windowHeight, "Minecraft!");
	app.runMainLoop();
}

