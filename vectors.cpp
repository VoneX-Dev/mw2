#include "vectors.hpp"
#include <math.h>
#include <algorithm>

vec3::vec3(void) {
	x = y = z = 0.0f;
}

vec3::vec3(float fx, float fy, float fz) {
	x = fx;
	y = fy;
	z = fz;
}

vec3::~vec3(void) {
};

void vec3::init(float ix, float iy, float iz) {
	x = ix; y = iy; z = iz;
}

vec3 vec3::clamp(void) {
	return vec3(x, y, z);
}

vec3 vec3::clamped() {
	vec3 clamped = *this;
	clamped.clamp();
	return clamped;
}

float vec3::normalize_float() {
	vec3 res = *this;
	float l = res.length();
	if (l != 0.0f) {
		res /= l;
	}
	else {
		res.x = res.y = res.z = 0.0f;
	}
	return l;
}

float vec3::distance_to(const vec3& other) {
	vec3 delta;
	delta.x = x - other.x;
	delta.y = y - other.y;
	delta.z = z - other.z;

	return delta.length();
}

void vec3::normalize(void) {
	auto vec_normalize = [&](vec3& v) {
		auto l = v.length();

		if (l != 0.0f) {
			v.x /= l;
			v.y /= l;
			v.z /= l;
		}
		else {
			v.x = v.y = 0.0f; v.z = 1.0f;
		}

		return l;
	};

	vec_normalize(*this);
}

vec3 vec3::normalized(void) {
	vec3 vec(*this);
	vec.normalize();

	return vec;
}

float vec3::length(void) {
	auto sqr = [](float n) {
		return static_cast<float>(n * n);
	};

	return sqrt(sqr(x) + sqr(y) + sqr(z));
}

float vec3::length_2d_sqr(void) const {
	return (x * x + y * y);
}

float vec3::dot(const vec3 other) {
	return (x * other.x + y * other.y + z * other.z);
}

float vec3::dot(const float* other) {
	const vec3& a = *this;

	return(a.x * other[0] + a.y * other[1] + a.z * other[2]);
}