#pragma once
#include <glm/ext/vector_float3.hpp>
#include <glm/fwd.hpp>
#include <string>
#include <unordered_map>
#include <variant>
#include <iostream>
#include "globals.hpp"
#include "utils.hpp"

namespace convars {
	using ConVar = std::variant<int, float, glm::vec3, bool>;

	inline std::unordered_map<std::string, ConVar> CONVARS;
    // Assign
    inline void set(const std::string& name, ConVar value) {
        CONVARS[name] = value;
    }

    inline void init() {
        set("viewHeight", 10.0f);
        set("maxSpeed", 16.0f);
        set("gravity", 3.6f);
		set("acceleration", 1.0f);
		set("sprintFac", 2.25f);
		set("airAcceleration", 5.5f);
		set("groundFriction", 8.0f);
		set("maxAirSpeed", 0.5f);
		set("gravity", 2.5f);
		set("jumpPower", 1.0f);
		set("autoBunnyHop", true);
		set("mSense", 2.0f);
		set("drawPos", false);
		set("exposure", 0.5f);
    }

    inline void printAll() {
	    for (auto& it : convars::CONVARS) {
	    	switch (it.second.index()) {
	     		case 0://int
		      		std::cout << it.first << ": " << std::get<int>(it.second) << "\n";
		      		break;
				case 1:
					std::cout << it.first << ": " << std::get<float>(it.second) << "\n";
		 			break;
				case 2:
					std::cout << it.first << ": ";
					print(std::get<glm::vec3>(it.second));
		 			break;
				case 3:
					std::cout << it.first << ": " << (std::get<bool>(it.second) ? "true" : "false") << "\n";
		 			break;
	     	}
	    }
    }
    // Get Variant as float
    inline float getFloat(const std::string& name) {
        if (CONVARS.count(name)) {
            return std::get<float>(CONVARS[name]);
        }
        return 0.0f;
    }
    // GetVariant as vec3
    inline glm::vec3 getVec3(const std::string& name) {
        if (CONVARS.count(name)) {
            return std::get<glm::vec3>(CONVARS[name]);
        }
        return glm::vec3(0);
    }
    inline bool getBool(const std::string& name) {
        if (CONVARS.count(name)) {
            return std::get<bool>(CONVARS[name]);
        }
        return false;
    }
    //make tell stuff to update the convars
    inline void push() {
    	R3D_ENVIRONMENT_SET(tonemap.exposure, convars::getFloat("exposure"));
	     // Setup bloom

	     R3D_ENVIRONMENT_SET(bloom.intensity, convars::getFloat("bloomStrength")*.001);
   		globals::player->updateConvars();
    }
}
