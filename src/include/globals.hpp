#pragma once
#include <raylib.h>
#include <r3d/r3d.h>

namespace globals {
	inline float iTime;
	inline float frametime;
	inline bool paused = false;
	inline int defaultiTimeLoc;
	inline int defaultViewLoc;

	inline void togglePause() {
		paused = !paused;
		if (paused) {
			EnableCursor();
		} else {
			DisableCursor();
		}
	}

	inline void update(Camera3D &camera) {
		if (!globals::paused && IsWindowFocused()) globals::togglePause();
		frametime = GetFrameTime();
		iTime += frametime;
	}
}
