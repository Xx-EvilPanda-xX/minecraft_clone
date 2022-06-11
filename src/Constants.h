#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <glm/glm.hpp>

namespace constants
{
	constexpr int windowWidth{ 1280 };
	constexpr int windowHeight{ 720 };

	constexpr int fullscreenWidth{ 1920 };
	constexpr int fullscreenHeight{ 1080 };

	constexpr bool fullscreen{ true };

	constexpr double mouse_sensitivity{ 0.025 };

	constexpr bool flatWorld{ false };
	constexpr int flatWorldHeight{ 64 };

	constexpr int renderDistance{ 14 };
	constexpr int hideDistance{ 12 };

	constexpr double playerReach{ 8.0f };
	constexpr double blockBreakCooldown{ 0.15 };
	constexpr double blockPlaceCooldown{ 0.15 };

	constexpr bool useTranslucentWater{ true };
	constexpr int waterLevel{ 48 };
	constexpr int beachSize{ 1 };

	constexpr double walkSpeed{ 3.5 };
	constexpr double sprintSpeed{ 9.0 };
	constexpr double playerDrift{ 20.0 };
	constexpr double playerSize{ 0.45 };
	constexpr double cameraFov{ 90.0 };
	constexpr glm::dvec3 playerSpawnPos{ 0.0, 96.0, 0.0 };

	constexpr double gravity{ 55.0 };
	constexpr double jumpHeight{ 1.31 };
}

#endif
