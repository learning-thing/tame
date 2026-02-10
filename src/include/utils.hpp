#pragma once
#include <glm/glm.hpp>
#include <raylib.h>
#include <raymath.h>


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
