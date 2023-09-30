#pragma once

namespace aimbot
{
	inline bool enabled = true;
	inline float fov = 0;
	inline bool crosshair = true;
	inline float crosshair_color[4]{ 1, 0, 0, 1 };
	inline float smoothing = 5;
}

namespace esp
{
	inline bool enabled = true;
	inline bool box = true;
	inline float box_color[4]{ 1, 1, 0, 1 };

	inline bool health = true;

	inline bool skeleton = false;
	inline float skeleton_color[4]{ 0, 1, 0, 1 };
}

namespace misc
{

}