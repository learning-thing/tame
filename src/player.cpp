#include <cstdio>
#include <glm/ext/vector_float3.hpp>
#include <glm/geometric.hpp>
#include <raylib.h>
#include <raymath.h>
#include "globals.hpp"
#include "player.hpp"
#include "convars.hpp"
#include "utils.hpp"

Player::Player() {
	health = 100;
	transform.translation = {-1, 1, 0};
	mv.m_flMaxSpeed = convars::getFloat("maxSpeed");
	view.offset = vec3(0, convars::getFloat("viewHeight"), 0);
	m_surfaceFriction = convars::getFloat("groundFriction");
	m_bOnGround = true;
	m_bSliding = false;
	view.targFov = 70;
	DisableCursor();
	mSense = convars::getFloat("mSense");
}

void Player::updatePlayer(Camera3D &camera) {
	if (globals::paused) return;
	mSense = convars::getFloat("mSense")/1000;
	//Sliding logic
	if (IsKeyDown(KEY_LEFT_CONTROL) && !m_bDucked && !globals::consoleActive) {
		m_bSliding = true;
	} else {
		m_bSliding = false;
	}

	fullWalkMove();
	transform.translation += mv.m_vecVelocity * globals::tickTime*60.0f;

	//Add addGravity
	if (transform.translation.y > 0) { addGravity(); m_bOnGround = false; }
	if (transform.translation.y <= 0)  {
		m_bOnGround = true;
		if (m_bOnGround && !m_bWasOnGround)  PlaySound(mv.groundHitSound);
		transform.translation.y = 0;
		mv.m_vecVelocity.y = 0;
	}
	m_bWasOnGround = m_bOnGround;
}

void Player::viewUpdate(Camera3D &camera) {
	if (globals::paused) return;
	//mSense = convars::getFloat("mSense");
	//float Sense = convars::getFloat("mSense");
	//print("sensitivity: ", Sense);
	//Vector2 mDelta = GetMouseDelta()*0.005f*(int)(!globals::paused);
	Vector2 mDelta = GetMouseDelta();
	view.rotation += vec2(mDelta.x, mDelta.y)*mSense;
	if (view.rotation.y > 3.14f) { view.rotation.y = 3.14f; }
	if (view.rotation.y < 0.1) { view.rotation.y = 0.1; }
	vec2 viewRotation = view.rotation + view.m_vecFxViewOffset_t*0.02f;
	//io::print((ZString)textFormat("View rotation: %f\n", view.rotation.y));

	if (m_bSliding) {
		view.offset.y = convars::getFloat("viewHeight")/5;
	} else {
		view.offset.y = convars::getFloat("viewHeight");
	}

	view.lookDir = vec3(sin(-viewRotation.x)*sin(viewRotation.y),
					    cos(viewRotation.y),
					    cos(-viewRotation.x)*sin(viewRotation.y));

	view.offset_t += (view.offset-view.offset_t)*globals::frametime*10.0f;
	camera.position = view.offset_t+transform.translation;

	float speed = glm::length(vec3(mv.m_vecVelocity.x, 0, mv.m_vecVelocity.z));
	distanceWalked += speed;
	//  -- Camera effects
	// fov
	view.targFov = 70 + glm::dot(vec3(view.lookDir.x, 0, view.lookDir.z), vec3(mv.m_vecVelocity.x, 0, mv.m_vecVelocity.z))*5;
	camera.fovy += (view.targFov-camera.fovy)*globals::frametime*20;
	// Camera "bounce"
	if (m_bSprinting) {
		view.m_vecFxViewOffset = vec2(sin(distanceWalked*0.005f), abs(cos(distanceWalked*0.01f)));
	} else {
		view.m_vecFxViewOffset = vec2(0);
	}
	view.m_vecFxViewOffset_t += (view.m_vecFxViewOffset-view.m_vecFxViewOffset_t)*globals::frametime*10.0f;
	camera.target = camera.position+view.lookDir;
}

void Player::friction() {
	bool isOnGround = true;

	float	speed = 0, newspeed = 0, control = 0;
	float	friction = 0;
	float	drop = 0;

	m_surfaceFriction = convars::getFloat("groundFriction");

	//Calculate speed
	speed = glm::length(mv.m_vecVelocity);

	if (speed < 1.0f) { m_bSprinting = false; }
	if (speed < 0.001f) { mv.m_vecVelocity = vec3(0, 0, 0); return; }

	if (isOnGround) { friction = m_surfaceFriction; }

	control = (speed < 0.01f) ? 0.01f : speed;
	if (!m_bSliding) { drop += control * friction * globals::tickTime; }
	else {drop += control * friction * globals::tickTime * 0.05f; }
	//io::printn((ZString)textFormat("Speed drop: %f\n", drop));
	newspeed = speed - drop;
	if (newspeed != speed) {
		newspeed /= speed;
		mv.m_vecVelocity *= newspeed;
	}
	mv.m_outWishVel -= mv.m_vecVelocity * (1.0f-newspeed);
}

void Player::fullWalkMove() {
	//Check if we need to apply ground friction
	m_bOnGround ? walkMove() : airMove();
}

void Player::walkMove() {
	//vec3 wishDir;
	vec3 wishVel;
	//float spd;
	//float fmove, smove;
	//int wishSpeed = 320;
	//vec3 forward, right, up;
	vec3 strafeDir = glm::cross(view.lookDir, vec3(0, 1, 0));

	if (!globals::consoleActive) {
		if (IsKeyDown(KEY_W) && !m_bSliding) {
			wishVel += view.lookDir;
			if (IsKeyDown(KEY_LEFT_SHIFT)) {
				m_bSprinting = true;
				printf("Started sprinting\n");
			}
		} else {
			m_bSprinting = false;
		}
		if (IsKeyDown(KEY_S)) { wishVel -= view.lookDir; }
		if (IsKeyDown(KEY_A)) { wishVel -= strafeDir; }
		if (IsKeyDown(KEY_D)) { wishVel += strafeDir; }
	}
	wishVel.y = 0;

	//Check if we want to sprint

	//Normalise velocity
	if (glm::length(wishVel) > 0) {//Avoid division by 0
		mv.m_outWishVel = glm::normalize(wishVel);
		if (m_bSliding) wishVel *= 0.001f;
	} else {//Reset wishVel
		mv.m_outWishVel = vec3(0, 0, 0);
	}
	//Apply friction
	friction();
	//Jump
	if (!globals::consoleActive) if (globals::keyPressed(globals::KEY_JUMP) || (convars::getBool("autoBunnyHop") && IsKeyDown(KEY_SPACE))) {
		mv.m_vecVelocity += vec3(0, convars::getFloat("jumpPower"), 0);
		//view.m_vecFxViewOffset_t += (Vector2){0, 10};
	}
	if (!m_bSprinting) accelerate(mv.m_outWishVel, mv.m_flMaxSpeed, (convars::getFloat("acceleration")));
	else accelerate(mv.m_outWishVel, mv.m_flMaxSpeed, convars::getFloat("acceleration")*convars::getFloat("sprintFac"));
	//transform.translation.y = 0;
}

void Player::accelerate(vec3 &wishdir, float wishSpeed, float accel) {
	float addSpeed, accelSpeed, currentSpeed;

	//See if we are chaging direction a bit
	currentSpeed = glm::dot(mv.m_vecVelocity, wishdir);

	//Reduce wishSpeed by the amount of veer
	addSpeed = wishSpeed - currentSpeed;

	//Skip of there's no speed to add
	if ( addSpeed <= 0) return;

	//Determine amount of acceleration
	accelSpeed = accel * globals::tickTime * wishSpeed * m_surfaceFriction;

	//Cap it
	if (accelSpeed > addSpeed) accelSpeed = addSpeed;

	//Adjust pmove vel
	mv.m_vecVelocity += wishdir * accelSpeed;
	//ZString dbgMsg = (ZString)textFormat("WishDir: %f %f %f\nCurrentSpeed: %f\nAddspeed: %f\nAccel speed: %f\nOutWishVel: %f %f %f\nPosition: %f %f %f\n", wishdir.x, wishdir.y, wishdir.z, currentSpeed, addSpeed, accelSpeed, mv.m_outWishVel.x, mv.m_outWishVel.y, mv.m_outWishVel.z, transform.translation.x, transform.translation.y, transform.translation.z);
	//io::print(dbgMsg);
}

void Player::airMove() {
	vec3 wishVel;
	vec3 strafeDir = cross(view.lookDir, vec3(0, 1, 0));

	if (IsKeyDown(KEY_W)) { wishVel += view.lookDir; }
	if (IsKeyDown(KEY_S)) { wishVel -= view.lookDir; }
	if (IsKeyDown(KEY_A)) { wishVel -= strafeDir; }
	if (IsKeyDown(KEY_D)) { wishVel += strafeDir; }

	wishVel.y = 0;

	if (glm::length(wishVel) > 0) {
		mv.m_outWishVel = glm::normalize(wishVel);
	} else {
		mv.m_outWishVel = vec3(0);
	}

	airAccelerate(mv.m_outWishVel, convars::getFloat("maxAirSpeed"), convars::getFloat("airAcceleration"));
	//transform.translation.y = 0;
}

void Player::airAccelerate(vec3 &wishdir, float wishSpeed, float accel) {
	float addSpeed, accelSpeed, currentSpeed;

	//See if we are chaging direction a bit
	currentSpeed = glm::dot(mv.m_vecVelocity, wishdir);

	//Reduce wishSpeed by the amount of veer
	addSpeed = wishSpeed - currentSpeed;

	//Skip of there's no speed to add
	if ( addSpeed <= 0) return;

	//Determine amount of acceleration
	accelSpeed = accel * globals::tickTime * wishSpeed * m_surfaceFriction;

	//Cap it
	if (accelSpeed > addSpeed) accelSpeed = addSpeed;

	//Adjust pmove vel
	mv.m_vecVelocity += wishdir * accelSpeed;
}

void Player::addGravity() {
	float ent_gravity = convars::getFloat("gravity");

	// Add gravity incorrectly
	mv.m_vecVelocity.y -= (ent_gravity * globals::tickTime);
}

bool Player::canAccelerate() {
	return true;
}
