#pragma once
#include <raylib.h>
#include <r3d/r3d.h>

namespace globals {
	inline const unsigned int TICKRATE = 100;
	inline float tickTime = 0;

	inline float iTime;
	inline float frametime;
	inline bool paused = false;

	inline void togglePause() {
		paused = !paused;
		if (paused) {
			EnableCursor();
		} else {
			DisableCursor();
		}
	}

	inline void update(Camera3D &camera) {
		//if (!globals::paused && IsWindowFocused()) globals::togglePause();
		frametime = GetFrameTime();
		iTime += frametime;
		tickTime+=frametime;
		if (IsKeyPressed(KEY_ESCAPE)) togglePause();
	}

	inline bool tick() {
		if (tickTime >= 1.0f/TICKRATE) {
			return true;
		}
		return false;
	}
}
