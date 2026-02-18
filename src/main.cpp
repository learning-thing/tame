#include "collision.hpp"
#include "convars.hpp"
#include "globals.hpp"
#include "r3d/r3d_ambient_map.h"
#include "r3d/r3d_draw.h"
#include "r3d/r3d_environment.h"
#include "r3d/r3d_importer.h"
#include "r3d/r3d_lighting.h"
#include "r3d/r3d_mesh.h"
#include <atomic>
#include <bits/chrono.h>
#include <cstdio>
#include <glm/ext/vector_float3.hpp>
#include <glm/geometric.hpp>
#include <r3d/r3d.h>
#include <raylib.h>
#include <raymath.h>
#include "r3d/r3d_model.h"
#include "tArray.hpp"
#include "utils.hpp"
#include "player.hpp"
#include "console.hpp"
#include <thread>

using namespace std::chrono_literals;

// Flag, um den Thread sauber zu beenden
std::atomic<bool> gameRunning(true);

void PhysicsTickLoop(Player* player, Camera3D& camera, tArray<hitBox>& hitBoxes, Console& console) {
    // 60 Ticks pro Sekunde = ~16.67ms pro Tick
    const auto tickrate = 8ms;

    while (gameRunning) {
        auto startTime = std::chrono::steady_clock::now();

        // Hier wird die Logik ausgeführt, die vorher in globals::tick() stand
        if (!globals::paused) {
            player->updatePlayer(camera);
            hitBoxes[0].update();
            globals::newTick();
        }

        auto endTime = std::chrono::steady_clock::now();
        auto duration = endTime - startTime;

        // Warten, bis der nächste Tick fällig ist
        if (duration < tickrate) {
            std::this_thread::sleep_for(tickrate - duration);
        }
    }
}

int main(int argc, char **argv) {
    InitWindow(1920, 1080, "Tame engine/game");
    SetWindowMonitor(0);
    InitAudioDevice();
    //SetWindowState(FLAG_WINDOW_RESIZABLE);

    SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor()));

    // Initialize R3D
    R3D_Init(GetScreenWidth(), GetScreenHeight());
    R3D_SetAntiAliasing(R3D_ANTI_ALIASING_FXAA);

    // Define procedural skybox parameters
    R3D_CubemapSky skyParams = R3D_CUBEMAP_SKY_BASE;
    skyParams.groundEnergy = 2.0f;
    skyParams.skyEnergy = 2.0f;
    skyParams.sunEnergy = 2.0f;
    R3D_Cubemap skyProcedural = R3D_GenCubemapSky(512, skyParams);
    R3D_AmbientMap ambientProcedural = R3D_GenAmbientMap(skyProcedural, R3D_AMBIENT_ILLUMINATION | R3D_AMBIENT_REFLECTION);

    // Setup environment sky
    R3D_Cubemap cubemap = R3D_LoadCubemap(RESOURCES_PATH "sky/indoor.hdr", R3D_CUBEMAP_LAYOUT_AUTO_DETECT);
    R3D_ENVIRONMENT_SET(background.skyBlur, 0.775f);
    R3D_ENVIRONMENT_SET(background.sky, cubemap);

    // Setup bloom
    R3D_ENVIRONMENT_SET(bloom.mode, R3D_BLOOM_MIX);
    R3D_ENVIRONMENT_SET(bloom.intensity, 0.075f);

    R3D_ENVIRONMENT_SET(background.sky, skyProcedural);
    R3D_ENVIRONMENT_SET(ambient.map, ambientProcedural);

    // Load model
    R3D_SetTextureFilter(TEXTURE_FILTER_ANISOTROPIC_4X);
    globals::model = R3D_LoadModelEx(MODELS_PATH "brutal.glb", R3D_IMPORT_MESH_DATA);

    // Setup camera
    Camera3D camera = {
        {0, 4, 20.5f},
        {0, 0, 0},
        {0, 1, 0},
        50
    };

    ToggleFullscreen();

    //R3D_Material mat = R3D_GetDefaultMaterial();
    //mat.albedo = R3D_LoadAlbedoMap(MATERIALS_PATH "ground/albedo.jpg", WHITE);
    //mat.normal = R3D_LoadNormalMap(MATERIALS_PATH "ground/normal.jpg", 100.0f);
    //mat.uvScale = {100.f, 100.f};

    R3D_Mesh groundPlane = R3D_GenMeshPlane(1000, 1000, 10, 10);

    SetExitKey(KEY_END);

    convars::init();

    // Setup tonemapping
    R3D_ENVIRONMENT_SET(tonemap.mode, R3D_TONEMAP_ACES);
    printf("%f", convars::getFloat("exposure"));
    R3D_ENVIRONMENT_SET(tonemap.exposure, convars::getFloat("exposure"));
    R3D_ENVIRONMENT_SET(tonemap.white, 4.0f);

    globals::player = new Player;
    convars::printAll();

    // Initialize window
    Console console;//Redirect everything into console from here

    tArray<hitBox> hitBoxes(10);
    hitBoxes.pushBack(hitBox(globals::player));

    // Main loop
    float speed = 0;
    float prevSpeed = 0;
    bool limitFPS = false;

    // Create directional light with shadows
    R3D_Light sun = R3D_CreateLight(R3D_LIGHT_DIR);
    R3D_SetLightDirection(sun, (Vector3){-1, -1, -1});
    R3D_SetLightActive(sun, true);
    R3D_SetLightRange(sun, 160.0f);
    R3D_SetShadowSoftness(sun, 2.0f);
    R3D_SetShadowDepthBias(sun, 0.01f);
    R3D_EnableShadow(sun);
    //globals::modelMatrix = MatrixMultiply(MatrixIdentity(), MatrixScale(10, 10, 10));
    //globals::modelMatrix = MatrixMultiply(globals::modelMatrix, MatrixTranslate(0, -30, 0));


    // Seperate tick thread
    std::thread physicsThread(PhysicsTickLoop, globals::player, std::ref(camera), std::ref(hitBoxes), std::ref(console));

    while (!WindowShouldClose()) {
   		globals::player->viewUpdate(camera);
    	console.update();
   		globals::update(camera);

		if (IsKeyPressed(KEY_Q)) {
			limitFPS = !limitFPS;
			limitFPS ? SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor())) : SetTargetFPS(0);
		}

        BeginDrawing();
            ClearBackground(BLACK);
            R3D_Begin(camera);
                //R3D_DrawMesh(groundPlane, mat, {0, -10, 0}, 1.0f);
                R3D_DrawModelPro(globals::model, globals::modelMatrix);
            R3D_End();
            DrawCircle(GetScreenWidth()/2, GetScreenHeight()/2, 2, BLACK);
            DrawCircle(GetScreenWidth()/2, GetScreenHeight()/2, 1, WHITE);
            DrawFPS(10, 10);

            if (convars::getBool("drawPos")) {
            	speed = globals::player->getSpeed();
            	DrawText(TextFormat("Chunk: %d %d\nPosition: \n%.2f \n%.2f\nSpeed: %02f", hitBoxes[0].getChunk().x, hitBoxes[0].getChunk().y, globals::player->transform.translation.x, globals::player->transform.translation.y, speed), 10, 100, 20, (prevSpeed < speed) ? GREEN : RED);
           		prevSpeed = speed;
            }
            console.draw();
        EndDrawing();
    }
    // Cleanup
    gameRunning = false;    // Thread signalisieren zu stoppen
    if (physicsThread.joinable()) {
        physicsThread.join(); // Warten, bis der Thread fertig ist
    }
    //R3D_UnloadModel(model, true);
    R3D_UnloadAmbientMap(ambientProcedural);
    R3D_UnloadCubemap(cubemap);
    R3D_Close();

    //dWorldDestroy(world);

    CloseAudioDevice();
    CloseWindow();

    return 0;
}
