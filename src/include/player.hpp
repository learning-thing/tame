#pragma once
#include <glm/ext/scalar_uint_sized.hpp>
#include <glm/ext/vector_float3.hpp>
#include <math.h>
#include <raylib.h>
#include <glm/glm.hpp>
#include "utils.hpp"

using namespace glm;

//Where the eyes are
struct PlayerView {
	vec3 offset_t = vec3(0);
	vec3 offset = vec3(0);
	vec2 rotation = vec2(0);
	vec3 lookDir = vec3(1, 0, 0);
	vec2 m_vecFxViewOffset = vec2(0);//Actual offset from effects
	vec2 m_vecFxViewOffset_t = vec2(0);//interpolated+#
	vec3 cameraTilt = vec3(0);
	float targFov = 70;
	float viewHeight;
};

struct CMoveData {
	vec3 m_vecVelocity = vec3(0);
	vec3 m_outWishVel = vec3(0);
	Sound groundHitSound = LoadSound(SOUNDS_PATH "land.ogg");
	//Convars
	float m_flMaxSpeed;
	uint8 autoBunnyHop;
	float acceleration;
	float sprintFac;
	float jumpPower;
	float maxAirSpeed;
	float airAcceleration;
};

class Player {
	float mSense = 0;
	public:
	Transform transform;
	PlayerView view;
	CMoveData mv;
	float m_flGravity = 0;
	float m_surfaceFriction;
	int health = 100;
	bool m_bOnGround = true;
	bool m_bWasOnGround = false;
	bool m_bSliding = false;
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
	void updateConvars();
	void viewUpdate(Camera3D &camera);
	void friction();
	void fullWalkMove();
	void walkMove();
	void accelerate(vec3 &wishdir, float wishSpeed, float accel);
	void airMove();
	void airAccelerate(vec3 &wishdir, float wishSpeed, float accel);
	void addGravity();
	bool canAccelerate();
	float getSpeed() const;
	Vector3 getPos() const;

	Player();
	~Player() = default;
};
