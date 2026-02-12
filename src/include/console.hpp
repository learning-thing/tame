#pragma once
#include <cstdio>
#include <iostream>
#include <raylib.h>
#include "convars.hpp"
#include "utils.hpp"
#include "globals.hpp"
#include "mujs/mujs.h"
#include <vector>
#include <string>
#include <sstream>

static void JsB_print(js_State *j) {
	std::cout << js_tonumber(j, 1) << "\n";
}
static void JsB_prints(js_State *j) {
	std::cout << js_tostring(j, 1) << "\n";
}

class Console {
	bool hidden = true;
	js_State *runtime;
	std::vector<std::string> history;
	int histPos = 0;
	std::stringstream coutbuffer;
	std::streambuf *old = std::cout.rdbuf(coutbuffer.rdbuf());

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
		history.push_back(cmdline);
		for (auto& it : history) {
			//printf("%s\n", it.c_str());
		}
		//printf("%d\n", histPos);
		cmdline.clear();
		updateConvars();
		//globals::togglePause();
		//hidden = true;
		histPos = 0;
		std::string text = coutbuffer.str(); // text will now contain "Bla\n"
	}

	public:
	std::string cmdline = "";
	int cursorPos = cmdline.length()-1;

	Console() {
		runtime = js_newstate(nullptr, nullptr, 0);
		registerJSFunc(JsB_print, "print");
		registerJSFunc(JsB_prints, "prints");
		js_dofile(runtime, SCRIPTS_PATH "start.js");
		updateConvars();
	}

	~Console() {
		std::cout.rdbuf(old);
	}

	void update() {

		if (!hidden) {
			if (globals::newKey != 0) {
				cmdline += globals::newKey;
				//printf("%c\n", globals::newKey);
			}
			if (IsKeyPressed(KEY_ESCAPE)) {
				hidden = true;
			}
			if (IsKeyPressed(KEY_BACKSPACE) && cmdline.length()) cmdline.pop_back();
			if (IsKeyPressed(KEY_ENTER)) run();
			if (IsKeyPressed(KEY_UP)) {
				if (history.size()-histPos > 0 && !history.empty()) {
					histPos++;
					cmdline = history[history.size()-histPos];
				}
			}
			if (IsKeyPressed(KEY_DOWN)) {
				if (histPos > 1) {
					histPos--;
					cmdline = history[history.size()-histPos];
				}
			}
			//printf("%d / %d: %s\n", histPos, (int)history.size(), cmdline.c_str());
			if (histPos > 1 && (history.size()-histPos) > 0 && !history.empty()) {
			}
			globals::consoleActive = !hidden;
			return;
		}
		if (globals::keyPressed(globals::KEY_CONSOLE_TOGGLE)) {
			hidden = !hidden;
			//globals::togglePause();
		}
	}
	void draw() {
		if (!hidden) {
			DrawRectangle(0, GetScreenHeight()-40, GetScreenWidth()/2, 40, ColorAlpha(BLACK, .4));
			DrawText((cmdline+( ((int)globals::iTime*2 % 2 == 0) ? "_" : " ")).c_str(), 0, GetScreenHeight()-30, 20, WHITE);
			//Draw Console output
			DrawText((coutbuffer.str()+"\n").c_str(), 0, 30, 20, WHITE);
		}
	}
};
