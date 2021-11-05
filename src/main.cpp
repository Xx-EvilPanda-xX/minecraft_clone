#include <iostream>
#include "Application.h"
#include "Constants.h"

int main()
{
	std::cout << "initializing...\n";

	Application app{};
	app.init(constants::windowWidth, constants::windowHeight, "Minecraft!");

	std::cout << "done!\n";

	app.runMainLoop();
}

