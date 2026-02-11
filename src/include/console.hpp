#pragma once
#include <iostream>
#include <raylib.h>
#include "convars.hpp"
#include "utils.hpp"
#include "globals.hpp"
#include "mujs/mujs.h"

static void JsB_print(js_State *j) {
	std::cout << js_tonumber(j, 1) << "\n";
}

class Console {
	bool hidden = true;
	js_State *runtime;

	void updateConvars() {
		for (auto& it : convars::CONVARS) {
			js_getglobal(runtime, it.first.c_str());
			//printf("Setting %s to ", it.first.c_str());
			switch (it.second.index()) {
				case 1:
					convars::set(it.first, (float)js_tonumber(runtime, -1));
					//printf("%f\n", convars::getFloat(it.first));
					break;
				case 3:
					convars::set(it.first, (bool)js_toboolean(runtime, -1));
					//printf("%s\n", convars::getBool(it.first) ? "true" : "false");
					break;
				default:
					break;
			}
		}
		//convars::printAll();
	}

	void run() {
		js_dostring(runtime, (cmdline+";\n").c_str());
		cmdline.clear();
		updateConvars();
		globals::togglePause();
	}

	public:
	std::string cmdline = "";
	int cursorPos = cmdline.length()-1;

	Console() {
		runtime = js_newstate(nullptr, nullptr, 0);
		registerJSFunc(JsB_print, "print");
		js_dofile(runtime, SCRIPTS_PATH "start.js");
		updateConvars();
	}
	void update() {
		if (!globals::paused) {
			hidden = true;
		}
		if (!hidden) {
			if (globals::newKey != 0) {
				cmdline += GetCharPressed();
				//printf("%c\n", globals::newKey);
			}
			if (IsKeyPressed(KEY_BACKSPACE) && cmdline.length()) cmdline.pop_back();
			if (IsKeyPressed(KEY_ENTER)) run();
			return;
		}
		if (globals::keyPressed(globals::KEY_CONSOLE_TOGGLE)) {
			hidden = !hidden;
			globals::togglePause();
		}
	}
	void draw() {
		if (!hidden) {
			DrawRectangle(0, GetScreenHeight()-40, GetScreenWidth()/2, 40, ColorAlpha(BLACK, .4));
			DrawText((cmdline+( ((int)globals::iTime % 2 == 0) ? "_" : " ")).c_str(), 0, GetScreenHeight()-30, 20, WHITE);
		}
	}
};
