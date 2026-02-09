#pragma once
#include <glm/ext/vector_float3.hpp>
#include <math.h>
#include <raylib.h>
#include <glm/glm.hpp>
#include "globals.hpp"
#include "convars.hpp"

using namespace glm;

//Where the eyes are
struct PlayerView {
	vec3 offset_t;
	vec3 offset;
	vec2 rotation;
	vec3 lookDir;
	vec2 m_vecFxViewOffset;//Actual offset from effects
	vec2 m_vecFxViewOffset_t;//interpolated+#
	vec3 cameraTilt;
	float targFov;
};

struct CMoveData {
	float m_flMaxSpeed;
	vec3 m_vecVelocity;
	vec3 m_outWishVel;
};

class Player {
	public:
	Transform transform;
	PlayerView view;
	CMoveData mv;
	float m_flGravity;
	float m_surfaceFriction;
	int health;
	bool m_bOnGround;
	bool m_bSliding;
	/*Later:
	- StartSlide (slide starting, not fully in slide yet)
	- Inslide (Fully in slide)
	*/
	//trying to crouch, which will result in sliding if landing or at high speed
	// otherwise we crouch. which offsets view downward and limits speed/aceleration
	bool m_bDucked;
	bool m_bBeginSliding;
	bool m_bCrouchRequested;
	bool m_bSprinting;
	float distanceWalked;

	void updatePlayer(Camera3D &camera);
	void friction();
	void fullWalkMove();
	void walkMove();
	void accelerate(vec3 &wishdir, float wishSpeed, float accel);
	void airMove();
	void airAccelerate(vec3 &wishdir, float wishSpeed, float accel);
	void addGravity();
	bool canAccelerate();

	Player();
	~Player();
};
