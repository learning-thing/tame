#include <cstdio>
#include <glm/ext/vector_float3.hpp>
#include <glm/geometric.hpp>
#include <raylib.h>
#include <raymath.h>
#include "globals.hpp"
#include "player.hpp"
#include "convars.hpp"
#include "r3d/r3d_kinematics.h"
#include "r3d/r3d_mesh_data.h"
#include "utils.hpp"

Player::Player() {
	health = 100;
	transform.translation = {-1, 1, 0};
	updateConvars();
	m_bOnGround = true;
	m_bSliding = false;
	view.targFov = 70;
	DisableCursor();
}

void Player::updatePlayer(Camera3D &camera) {
	if (globals::paused) return;
	//Sliding logic
	if (IsKeyDown(KEY_LEFT_CONTROL) && !m_bDucked && !globals::consoleActive) {
		m_bSliding = true;
	} else {
		m_bSliding = false;
	}

	fullWalkMove();
	transform.translation += mv.m_vecVelocity * globals::tickTime*10.0f;

	updateGround();

	//Add addGravity
	if (!m_bOnGround) { addGravity();}
	else {
		if (m_bOnGround && !m_bWasOnGround)  PlaySound(mv.groundHitSound);
		//transform.translation.y += 1;
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
		view.offset.y = view.viewHeight/10;
	} else {
		view.offset.y = view.viewHeight;
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
	view.targFov = 70 + abs(glm::dot(vec3(view.lookDir.x, 0, view.lookDir.z), vec3(mv.m_vecVelocity.x, 0, mv.m_vecVelocity.z)))*2;
	camera.fovy += (view.targFov-camera.fovy)*globals::frametime*5;

	// Camera "bounce"
	view.m_vecFxViewOffset_t += (view.m_vecFxViewOffset-view.m_vecFxViewOffset_t)*globals::frametime*10.0f;
	camera.target = camera.position+view.lookDir;
}

void Player::friction() {
	float	speed = 0, newspeed = 0, control = 0;
	float	friction = 0;
	float	drop = 0;

	//Calculate speed
	speed = glm::length(mv.m_vecVelocity);

	if (speed < 1.0f) { m_bSprinting = false; }
	if (speed < 0.001f) { mv.m_vecVelocity = vec3(0, 0, 0); return; }

	friction = m_surfaceFriction;
	//friction = 0;

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
	vec3 wishVel = vec3(0);
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
				//printf("Started sprinting\n");
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
	if (!globals::consoleActive) if (globals::keyPressed(globals::KEY_JUMP) || (mv.autoBunnyHop && IsKeyDown(KEY_SPACE))) {
		mv.m_vecVelocity.y += mv.jumpPower;
		//view.m_vecFxViewOffset_t += (Vector2){0, 10};
	}
	if (m_bSprinting) {
		accelerate(mv.m_outWishVel, mv.m_flMaxSpeed, mv.acceleration*mv.sprintFac);
		return;
	}
	if (m_bSliding) {
		accelerate(mv.m_outWishVel, mv.m_flMaxSpeed*.25f, (mv.acceleration*.25f));
		return;
	}
	accelerate(mv.m_outWishVel, mv.m_flMaxSpeed, mv.acceleration);
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
	accelSpeed = accel * globals::tickTime * wishSpeed;

	//Cap it
	if (accelSpeed > addSpeed) accelSpeed = addSpeed;

	//Adjust pmove vel
	mv.m_vecVelocity += wishdir * accelSpeed;
}

void Player::airMove() {
	vec3 wishVel = vec3(0, 0, 0);
	vec3 strafeDir = cross(view.lookDir, vec3(0, 1, 0));

	if (IsKeyDown(KEY_W)) { wishVel += view.lookDir; }
	if (IsKeyDown(KEY_S)) { wishVel -= view.lookDir; }
	if (IsKeyDown(KEY_A)) { wishVel -= strafeDir; }
	if (IsKeyDown(KEY_D)) { wishVel += strafeDir; }

	wishVel.y = 0;

	if (glm::length(wishVel) > 0.0001f) {
	    mv.m_outWishVel = glm::normalize(wishVel);
	} else {
	    mv.m_outWishVel = vec3(0.0f);
	}

	if (m_bSliding) {
		airAccelerate(mv.m_outWishVel, mv.maxAirSpeed*.25f, mv.airAcceleration*.25f);
		return;
	}
	//print(wishVel);
	airAccelerate(mv.m_outWishVel, mv.maxAirSpeed, mv.airAcceleration);
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
	accelSpeed = accel * globals::tickTime * wishSpeed;

	//Cap it
	if (accelSpeed > addSpeed) accelSpeed = addSpeed;

	//Adjust pmove vel
	mv.m_vecVelocity += wishdir * accelSpeed;
}

void Player::addGravity() {;
	// Add gravity incorrectly (could be better idk)
	mv.m_vecVelocity.y -= (m_flGravity * globals::tickTime);
}

void Player::updateConvars() {
	mv.m_flMaxSpeed = convars::getFloat("maxSpeed");
	m_surfaceFriction = convars::getFloat("groundFriction");
	mSense = convars::getFloat("mSense")/1000;
	view.offset = vec3(0, convars::getFloat("viewHeight"), 0);
	m_flGravity = convars::getFloat("gravity");
	mv.sprintFac = convars::getFloat("sprintFac");
	mv.acceleration = convars::getFloat("acceleration");
	mv.maxAirSpeed = convars::getFloat("maxAirSpeed");
	mv.airAcceleration = convars::getFloat("airAcceleration");
	mv.autoBunnyHop = convars::getBool("autoBunnyHop");
	mv.jumpPower = convars::getFloat("jumpPower");
	view.viewHeight = convars::getFloat("viewHeight");
}

void Player::updateGround() {
	groundCheck.colray = (Ray){
		.position = transform.translation+(Vector3){0, view.viewHeight, 0},
		.direction = (Vector3){0, -1, 0},
	};
	groundCheck.groundCollision = R3D_RaycastModel(groundCheck.colray, globals::model, globals::modelMatrix);

	//std::cout << groundCheck.groundCollision.distance << "\n";
	if (groundCheck.groundCollision.hit) {
		print(groundCheck.groundCollision.point);
		if (groundCheck.groundCollision.point.y >= transform.translation.y) {
			m_bOnGround = true;
			transform.translation.y = groundCheck.groundCollision.point.y;
			return;
		}
		m_bOnGround = false;
	}
}

bool Player::canAccelerate() { return true; }

float Player::getSpeed() const {
	vec3 s = mv.m_vecVelocity;
	s.y = 0;
	return glm::length(s);
}

Vector3 Player::getPos() const {
	return transform.translation;
}
