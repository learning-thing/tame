#pragma once
#include <iostream>
#include <istream>
#include <raylib.h>
#include <streambuf>
#include <string>
#include <sstream>
#include "convars.hpp"
#include "r3d/r3d_model.h"
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
static void JsB_loadModelMap(js_State *j) {
	static const std::string newModelPath = std::string(MODELS_PATH)+js_tostring(j, 1);
	js_pop(j, 2);
	std::cout << "Trying to load " << newModelPath << "\n";

	R3D_UnloadModel(globals::model, true);
	globals::model = R3D_LoadModel(newModelPath.c_str());
}

class Console {
    bool hidden = true;
    js_State *runtime;
    tArray<std::string> history = 10;    // history
    tArray<std::string> outputLog = 25;  // array of cout lines (rolling log buffer)
    int histPos = 0;

    std::stringstream coutbuffer;
    std::streambuf *old = std::cout.rdbuf(coutbuffer.rdbuf());
    std::istream stream;

    float backSpaceDown = 0;

    void updateConvars() {
    	//Read convars from js runtime
     //
        for (auto& it : convars::CONVARS) {
            js_getglobal(runtime, it.first.c_str());
            switch (it.second.index()) {
                case 1: it.second = (float)js_tonumber(runtime, -1); break;
                case 3: it.second = (bool)js_toboolean(runtime, -1); break;
                default: break;
            }
            js_pop(runtime, 1);
        }
        convars::push();
    }

    void run() {
        if (cmdline.empty()) return;

        js_dostring(runtime, (cmdline + ";\n").c_str());
        history.pushBack(cmdline);
        cmdline.clear();
        updateConvars();
        histPos = 0;
    }

public:
    std::string cmdline;

    Console() : stream(coutbuffer.rdbuf()) { // Initialisierungsliste für den Stream
        runtime = js_newstate(nullptr, nullptr, 0);
        registerJSFunc(JsB_print, "print");
        registerJSFunc(JsB_prints, "prints");
        registerJSFunc(JsB_loadModelMap, "map");
        js_dofile(runtime, SCRIPTS_PATH "start.js");
        updateConvars();
        convars::push();
    }

    ~Console() {
        std::cout.rdbuf(old);
        js_freestate(runtime);
    }

    void update() {
        // NEU: Den Buffer auslesen und in das outputLog schieben
        std::string line;
        while (std::getline(stream, line)) {
            outputLog.pushBack(line);
            // Begrenzung auf die neuesten 20 Zeilen
            if (outputLog.size() > 20) {
                outputLog.remove(0);
            }
        }
        stream.clear(); // EOF-Bit zurücksetzen, um im nächsten Frame weiterlesen zu können

        if (!hidden) {
            const char newKey = GetCharPressed();
            if (newKey != 0) cmdline += newKey;

            if (IsKeyPressed(KEY_ESCAPE)) {
            	hidden = true;
            	globals::togglePause();
            }

            if (IsKeyPressed(KEY_BACKSPACE) && !cmdline.empty()) cmdline.pop_back();

            if (IsKeyDown(KEY_BACKSPACE)) {
                backSpaceDown += globals::frametime;
                if (backSpaceDown >= .2f && ((int)(globals::iTime * 100) % 2 == 0)) {
                    if (!cmdline.empty()) cmdline.pop_back();
                }
            }
            if (IsKeyReleased(KEY_BACKSPACE)) backSpaceDown = 0;
            if (IsKeyPressed(KEY_ENTER)) run();
            // Historie-Navigation
            if (IsKeyPressed(KEY_UP)) {
                if (histPos < history.size() && !history.empty()) {
                    histPos++;
                    cmdline = history[history.size() - histPos];
                }
            }
            if (IsKeyPressed(KEY_DOWN)) {
                if (histPos > 1) {
                    histPos--;
                    cmdline = history[history.size() - histPos];
                } else if (histPos == 1) {
                    histPos = 0;
                    cmdline.clear();
                }
            }
            globals::consoleActive = !hidden;
        } else if (IsKeyPressed(KEY_T)) {
            hidden = false;
        }
    }

    void draw() {
        if (!hidden) {
            // Hintergrund für die Eingabezeile
            DrawRectangle(0, GetScreenHeight()/2, GetScreenWidth() / 2, GetScreenHeight()/2, ColorAlpha(BLACK, .6f));
            DrawRectangle(0, GetScreenHeight() - 40, GetScreenWidth() / 2, 40, ColorAlpha(BLACK, .6f));

            // Cursor-Blinken und Eingabetext
            std::string cursor = ((int)(globals::iTime * 2.0f) % 2 == 0) ? "_" : " ";
            DrawText((cmdline + cursor).c_str(), 10, GetScreenHeight() - 30, 20, GRAY);

            // NEU: Konsolen-Historie zeichnen (von unten nach oben)
            for (int i = 0; i < outputLog.size(); i++) {
                // Wir zeichnen die Zeilen über der Eingabezeile
                // outputLog[size-1] ist die neueste Nachricht
                int yPos = (GetScreenHeight() - 70) - (outputLog.size() - 1 - i) * 22;
                DrawText(outputLog[i].c_str(), 10, yPos, 18, LIGHTGRAY);
            }
        }
    }
};
