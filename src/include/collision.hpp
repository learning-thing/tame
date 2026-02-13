#pragma once
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_int2.hpp>
#include <glm/glm.hpp>
#include <player.hpp>
#include <raylib.h>



class hitBox {
	glm::vec3 position;
	glm::vec3 scale;
	BoundingBox bbox;
	Player *owner = nullptr;
	public:
	hitBox() : owner(nullptr) {

	}
	hitBox(Player *player) : owner(player) {
		position = vec3(0, 0, 0);
	}
	void update() {
		position = glm::vec3(owner->transform.translation.x, owner->transform.translation.y, owner->transform.translation.z);
	}
	glm::ivec2 getChunk() {
		return glm::ivec2(int(position.x/10), int(position.z/10));
	}
	bool collidesWith (hitBox& hb) {
		//Check the chunk x, then chunk y
		// only if same, calculate distance
		auto myChunk = getChunk();
		auto hbChunk = hb.getChunk();
		if (hbChunk.x != myChunk.x) return false;
		if (hbChunk.y != myChunk.y) return false;


		return false;
	}
};
