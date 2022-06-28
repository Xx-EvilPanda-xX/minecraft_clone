#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <glm/glm.hpp>

namespace constants
{
	inline int windowWidth{ 1280 };
	inline int windowHeight{ 720 };

	inline int fullscreenWidth{ 1920 };
	inline int fullscreenHeight{ 1080 };

	inline bool fullscreen{ false };

	inline double mouseSensitivity{ 0.025 };

	inline bool flatWorld{ false };
	inline int flatWorldHeight{ 64 };

	inline int loadDistance{ 14 };
	inline int renderDistance{ 12 };

	inline double playerReach{ 8.0 };
	inline double blockBreakCooldown{ 0.15 };
	inline double blockPlaceCooldown{ 0.15 };

	inline bool useTranslucentWater{ true };
	inline int waterLevel{ 48 };
	inline int beachSize{ 1 };

	inline double walkSpeed{ 3.5 };
	inline double sprintSpeed{ 9.0 };
	inline double playerDrift{ 20.0 };
	inline double playerSize{ 0.45 };
	inline double cameraFov{ 90.0 };

	inline double gravity{ 55.0 };
	inline double jumpHeight{ 1.31 };
}

#endif
