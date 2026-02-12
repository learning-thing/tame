#include "convars.hpp"
#include "globals.hpp"
#include "r3d/r3d_ambient_map.h"
#include "r3d/r3d_draw.h"
#include "r3d/r3d_environment.h"
#include "r3d/r3d_material.h"
#include "r3d/r3d_mesh.h"
#include <glm/ext/vector_float3.hpp>
#include <glm/geometric.hpp>
#include <iostream>
#include <r3d/r3d.h>
#include <raylib.h>
#include <raymath.h>
#include "utils.hpp"
#include "player.hpp"
#include "console.hpp"

int main(int argc, char **argv) {
    // Initialize window
    Console console;//Redirect everything into console from here
    //
    InitWindow(1920, 1080, "[r3d] - PBR example");
    InitAudioDevice();
    SetWindowState(FLAG_WINDOW_RESIZABLE);


    //SetTargetFPS(100);

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
    //R3D_ENVIRONMENT_SET(bloom.mode, R3D_BLOOM_MIX);
    //R3D_ENVIRONMENT_SET(bloom.intensity, 0.02f);

    R3D_ENVIRONMENT_SET(background.sky, skyProcedural);
    R3D_ENVIRONMENT_SET(ambient.map, ambientProcedural);

    // Setup tonemapping
    R3D_ENVIRONMENT_SET(tonemap.mode, R3D_TONEMAP_ACES);
    R3D_ENVIRONMENT_SET(tonemap.exposure, 0.5f);
    R3D_ENVIRONMENT_SET(tonemap.white, 4.0f);

    // Load model
    R3D_SetTextureFilter(TEXTURE_FILTER_ANISOTROPIC_4X);
    R3D_Model model = R3D_LoadModel(RESOURCES_PATH "models/DamagedHelmet.glb");

    // Setup camera
    Camera3D camera = {
        .position = {0, 4, 20.5f},
        .target = {0, 0, 0},
        .up = {0, 1, 0},
        .fovy = 60
    };

    ToggleFullscreen();

    R3D_Material mat = R3D_GetDefaultMaterial();
    mat.albedo = R3D_LoadAlbedoMap(MATERIALS_PATH "ground/albedo.jpg", WHITE);
    mat.normal = R3D_LoadNormalMap(MATERIALS_PATH "ground/normal.jpg", 1.0f);
    mat.uvScale = {10.f, 10.f};

    R3D_Mesh groundPlane = R3D_GenMeshPlane(1000, 1000, 10, 10);

    SetExitKey(KEY_END);

    convars::init();

    Player player;

    convars::printAll();

    Matrix modelMatrix = MatrixIdentity();
    modelMatrix = MatrixMultiply(MatrixIdentity(), MatrixScale(50, 50, 50));
    modelMatrix = MatrixMultiply(modelMatrix, MatrixTranslate(0, 25, 0));

    // Main loop
    float speed = 0;
    float prevSpeed = 0;
    bool limitFPS = false;

    while (!WindowShouldClose()) {
   		player.viewUpdate(camera);
   		globals::update(camera);
     	//Dumbass Tick-system
      	console.update();
	    if (globals::tick()) {
			prevSpeed = speed;
			player.updatePlayer(camera);
			speed = glm::length(player.mv.m_vecVelocity);
			globals::newTick();
	    }

		if (IsKeyPressed(KEY_Q)) {
			limitFPS = !limitFPS;
			limitFPS ? SetTargetFPS(100) : SetTargetFPS(0);
		}

        BeginDrawing();
            ClearBackground(BLACK);
            R3D_Begin(camera);
                R3D_DrawMesh(groundPlane, mat, {0, -10, 0}, 1.0f);
                //R3D_DrawModelPro(model, modelMatrix);
            R3D_End();
            DrawCircle(GetScreenWidth()/2, GetScreenHeight()/2, 2, BLACK);
            DrawCircle(GetScreenWidth()/2, GetScreenHeight()/2, 1, WHITE);
            DrawFPS(10, 10);

            DrawText(TextFormat("Speed: %02f", speed), 10, 100, 20, (prevSpeed < speed) ? GREEN : RED);
            console.draw();
        EndDrawing();
    }

    // Cleanup
    R3D_UnloadModel(model, true);
    R3D_UnloadAmbientMap(ambientProcedural);
    R3D_UnloadCubemap(cubemap);
    R3D_Close();

    CloseAudioDevice();
    CloseWindow();

    return 0;
}
