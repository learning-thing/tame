#pragma once
#include <iostream>
#include <istream>
#include <raylib.h>
#include <streambuf>
#include <string>
#include <sstream>
#include "convars.hpp"
#include "utils.hpp"
#include "globals.hpp"
#include "mujs/mujs.h"
#include "tArray.hpp"

static void JsB_print(js_State *j) {
	std::cout << js_tonumber(j, 1) << "\n";
}
static void JsB_prints(js_State *j) {
	std::cout << js_tostring(j, 1) << "\n";
}

class Console {
	bool hidden = true;
	js_State *runtime;
	tArray<std::string> history = 10;
	int histPos = 0;
	std::stringstream coutbuffer;
	std::streambuf *old = std::cout.rdbuf(coutbuffer.rdbuf());
	std::istream strem(coutbuffer.rdbuf());
	float backSpaceDown = 0;

	void updateConvars() {
		for (auto& it : convars::CONVARS) {
			js_getglobal(runtime, it.first.c_str());
			//printf("Setting %s to ", it.first.c_str());
			switch (it.second.index()) {
				case 1:
					it.second = (float)js_tonumber(runtime, -1);
					//printf("%f\n", convars::getFloat(it.first));
					break;
				case 3:
					it.second = (bool)js_toboolean(runtime, -1);
					//printf("%s\n", convars::getBool(it.first) ? "true" : "false");
					break;
				default:
					break;
			}
			js_pop(runtime, 1);
		}
		convars::push();
		//convars::printAll();
	}

	void run() {
		js_dostring(runtime, (cmdline+";\n").c_str());
		history.pushBack(cmdline.c_str());
		//if (history.size() > 10) history.pop_front();
		//printf("%d\n", histPos);
		cmdline.clear();
		updateConvars();
		//globals::togglePause();
		//hidden = true;
		histPos = 0;
		std::string text = coutbuffer.str(); // text will now contain "Bla\n"
	}

	public:
	std::string cmdline;
	int cursorPos = cmdline.length()-1;

	Console() {
		runtime = js_newstate(nullptr, nullptr, 0);
		registerJSFunc(JsB_print, "print");
		registerJSFunc(JsB_prints, "prints");
		js_dofile(runtime, SCRIPTS_PATH "start.js");
		updateConvars();
		convars::push();
	}

	~Console() {
		std::cout.rdbuf(old);
	}

	void update() {
		if (!hidden) {
			const char newKey = GetCharPressed();
			if (newKey != 0) {
				cmdline += newKey;
				//printf("%c\n", globals::newKey);
			}
			if (IsKeyPressed(KEY_ESCAPE)) {
				hidden = true;
			}
			if (IsKeyPressed(KEY_BACKSPACE) && cmdline.length()) {
				cmdline.pop_back();
			}
			if (IsKeyDown(KEY_BACKSPACE)) {
				backSpaceDown += globals::frametime;
				//std::cout << backSpaceDown << "\n";
				if ( backSpaceDown >= .2f && ((int)(globals::iTime*100 ) % 2 == 0)) {
					if (!cmdline.empty()) {
						cmdline.pop_back();
					}
				}
			}
			if (IsKeyReleased(KEY_BACKSPACE)) backSpaceDown = 0;
			if (IsKeyPressed(KEY_ENTER)) run();
			if (IsKeyPressed(KEY_UP)) {
				if (histPos < history.size() && !history.empty()) {
					//std::cout << history.size() << "\n";
					//std::cout << histPos << "\n";
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
			/*
			if (histPos > 1 && (history.size()-histPos) > 0 && !history.empty()) {
			} */
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
			DrawText((cmdline+( ((int)globals::iTime*100 % 3 == 0) ? "_" : " ")).c_str(), 0, GetScreenHeight()-30, 20, WHITE);
			//Draw Console output
			DrawText((coutbuffer.str()+"\n").c_str(), 3, 30, 20, WHITE);
		}
	}
};
