#include <iostream>
#include "Application.h"
#include "Constants.h"

int main()
{
	std::cout << "initializing...\n";

	Application app{ constants::windowWidth, constants::windowHeight, "Minecraft!" };
	app.init();

	std::cout << "done!\n";

	app.runMainLoop();
}

