#pragma once
#include <cstdint>
#include <cstdio>
#include <raylib.h>
#include <r3d/r3d.h>

namespace globals {
	enum tickedKeys {
		KEY_FD = (1 << 0),
		KEY_BW = (1 << 1),
		KEY_LEFT = (1 << 2),
		KEY_RIGHT = (1 << 3),
		KEY_JUMP = (1 << 4),
		KEY_CONSOLE_TOGGLE = (1 << 5)
	};

	inline uint64_t lastTickKeys = 0;

	inline const unsigned int TICKRATE = 100;
	inline float tickTime = 0;
	inline unsigned long ticksDone = 0;

	inline float iTime;
	inline float frametime;
	inline bool paused = false;


	inline char newKey;
	static char lastKey;

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

		//update tick keys
		lastKey = newKey;
		newKey = GetCharPressed();
		//if (lastKey != newKey) printf("new key: %d\n", newKey);
		switch (newKey) {
			case 't':
				lastTickKeys |= KEY_CONSOLE_TOGGLE;
				break;
			case ' ':
				lastTickKeys |= KEY_JUMP;
				newKey = 0;
				break;
			default:
				//printf("KEY PRESSED: %d");
				break;
		}
	}

	inline bool keyPressed(tickedKeys key) {
		return lastTickKeys & key;
	}

	inline bool tick() {
		if (tickTime >= 1.0f/TICKRATE) return true;
		return false;
	}

	inline void newTick() {
		tickTime = 0;
		ticksDone++;
		lastTickKeys = 0;
	}
}
