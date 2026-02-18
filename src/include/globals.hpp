#pragma once
#include "player.hpp"
#include "r3d/r3d_model.h"
#include <cstdint>
#include <glm/fwd.hpp>
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

	inline uint64_t tickKeys = 0;
	inline uint64_t prevTickKeys = 0;

	inline const unsigned int TICKRATE = 100;
	inline float tickTime = 0;
	inline unsigned long ticksDone = 0;

	inline float iTime;
	inline float frametime;
	inline bool paused = false;
	inline bool consoleActive = false;

	inline char newKey;
	static char lastKey;

	inline Player *player;
	inline R3D_Model model;
	inline Matrix modelMatrix = MatrixIdentity();

	inline void togglePause() {
		paused = !paused;
		if (paused) {
			EnableCursor();
		} else {
			DisableCursor();
		}
	}

	inline void update(Camera3D &camera) {
		if (!globals::paused && !IsWindowFocused()) globals::togglePause();
		frametime = GetFrameTime();
		iTime += frametime;
		tickTime+=frametime;
		if (IsKeyPressed(KEY_ESCAPE) && !consoleActive) togglePause();

		tickKeys |= KEY_JUMP * IsKeyDown(KEY_SPACE);
		tickKeys |= KEY_CONSOLE_TOGGLE * IsKeyDown(KEY_T);
	}

	inline bool keyPressed(tickedKeys key) {
		return tickKeys & key && !(prevTickKeys & key);
	}

	inline uint8 tick() {
		if (tickTime >= 1.0f/TICKRATE) return true;
		return false;
	}

	inline void newTick() {
		tickTime = 0.0f;
		ticksDone++;
		prevTickKeys = tickKeys;
		tickKeys = 0;
	}
}
