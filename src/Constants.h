#ifndef CONSTANTS_H
#define CONSTANTS_H

namespace constants
{
	constexpr int windowWidth{ 1280 };
	constexpr int windowHeight{ 720 };

	constexpr int fullscreenWidth{ 1920};
	constexpr int fullscreenHeight{ 1080 };

	constexpr bool fullscreen{ false };

	constexpr float mouse_sensitivity{ 0.035f };

	constexpr bool flatWorld{ false };
	constexpr int flatWorldHeight{ 64 };

	constexpr int renderDistance{ 10 };

	constexpr float playerReach{ 100.0f };
	constexpr float blockBreakCooldown{ 0.015f };
	constexpr float blockPlaceCooldown{ 0.015f };

	constexpr int waterLevel{ 48 };
}

#endif
