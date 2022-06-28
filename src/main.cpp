#include <iostream>
#include "Application.h"
#include "Constants.h"
#include "Config.h"

int main()
{
	std::cout << "initializing...\n";

	Config::loadAllConfigurations(std::filesystem::path{ "config.txt" });
	Application app{ constants::windowWidth, constants::windowHeight, "Minecraft!" };
	app.init();

	std::cout << "done!\n";

	app.run();
}

