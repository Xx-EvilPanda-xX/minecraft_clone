#include <iostream>
#include "Application.h"
#include "Constants.h"

int main()
{
	std::cout << "initializing...\n";

	ChunkManager chunkManager{ nullptr };

	Application app{ constants::windowWidth, constants::windowHeight, "Minecraft!", chunkManager };
	app.init();

	std::cout << "done!\n";

	app.runMainLoop();
}

