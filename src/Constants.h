#ifndef CONSTANTS_H
#define CONSTANTS_H

namespace constants
{
	constexpr int windowWidth{ 1280 };
	constexpr int windowHeight{ 720 };

	constexpr int fullscreenWidth{ 1920 };
	constexpr int fullscreenHeight{ 1080 };

	constexpr bool fullscreen{ false };

	constexpr float mouse_sensitivity{ 0.035f };

	constexpr bool flatWorld{ false };
	constexpr int flatWorldHeight{ 64 };

	constexpr int renderDistance{ 10 };

	constexpr float playerReach{ 8.0f };
	constexpr float blockBreakCooldown{ 0.15f };
	constexpr float blockPlaceCooldown{ 0.15f };

	constexpr int waterLevel{ 48 };

	constexpr float playerDrift{ 20.0f };
	constexpr float playerSize{ 0.45f };

	constexpr float gravity{ 60.0f };
	constexpr float jumpHeight{ 1.75f };
}

#endif
