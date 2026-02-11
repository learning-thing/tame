#pragma once
#include <glm/glm.hpp>
#include <raylib.h>
#include <raymath.h>
#include <iostream>

#ifndef RESOURCES_PATH
#define RESOURCES_PATH "res/"
#define MODELS_PATH RESOURCES_PATH "models/"
#define SCRIPTS_PATH RESOURCES_PATH "scripts/"
#define MATERIALS_PATH RESOURCES_PATH "materials/"
#define SOUNDS_PATH RESOURCES_PATH "sounds/"

#endif

#define registerJSFunc(from, to) \
js_newcfunction(runtime, from, to, 0); \
js_setglobal(runtime, to)

using namespace glm;


inline void operator+=(Vector3& a, const vec3 &b) {
	a.x+=b.x;
	a.y+=b.y;
	a.z+=b.z;
}

inline Vector3 operator+(Vector3& a, vec3 &b) {
	return {a.x+b.x, a.y+b.y, a.z+b.z};
}

inline Vector3 operator+(vec3& a, Vector3 &b) {
	return {a.x+b.x, a.y+b.y, a.z+b.z};
}

inline Vector3 operator*=(const Vector3& a, const vec3 &b) {//is actually =
	return {b.x, b.y, b.z};
}

inline vec3 operator*=(const vec3& a, const Vector3 &b) {//is actually =
	return {b.x, b.y, b.z};
}

inline Vector2 operator*=(const Vector2& a, const vec2 &b) {//is actually =
	return {b.x, b.y};
}


inline void print(vec3 v) {
	std::cout << v.x << " " << v.y << " " << v.z << "\n";
}

inline void print(Vector3 v) {
	std::cout << v.x << " " << v.y << " " << v.z << "\n";
}

inline void print(const char *msg, float f) {
	std::cout << msg << f << "\n";
}
