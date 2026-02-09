#pragma once
#include <glm/ext/vector_float3.hpp>
#include <string>
#include <unordered_map>

union ConVar {
	int asInt;
	float asFloat;
	glm::vec3 asVec3;
	bool asBool;
};

static std::unordered_map<std::string, ConVar> CONVARS;

namespace convars {
	inline ConVar get(const char *name) {
		return CONVARS[name];
	}

	inline void init() {

	}
}
