#pragma once
#include <math.h>

class vec3 {
public:
	vec3();
	vec3(float, float, float);
	~vec3();

	float x, y, z;

	vec3& operator+=(const vec3& v) {
		x += v.x; y += v.y; z += v.z; return *this;
	}
	vec3& operator-=(const vec3& v) {
		x -= v.x; y -= v.y; z -= v.z; return *this;
	}
	vec3& operator*=(float v) {
		x *= v; y *= v; z *= v; return *this;
	}
	bool operator==(const vec3& v) {
		return (x == v.x) && (y == v.y);
	}
	vec3 operator+(const vec3& v) {
		return vec3{ x + v.x, y + v.y, z + v.z };
	}
	vec3 operator-(const vec3& v) {
		return vec3{ x - v.x, y - v.y, z - v.z };
	}
	vec3 operator*(float fl) const {
		return vec3(x * fl, y * fl, z * fl);
	}
	vec3 operator*(const vec3& v) const {
		return vec3(x * v.x, y * v.y, z * v.z);
	}
	vec3& operator/=(float fl) {
		x /= fl;
		y /= fl;
		z /= fl;
		return *this;
	}
	auto operator-(const vec3& other) const -> vec3 {
		auto buf = *this;

		buf.x -= other.x;
		buf.y -= other.y;
		buf.z -= other.z;

		return buf;
	}

	auto operator/(float other) const {
		vec3 vec;
		vec.x = x / other;
		vec.y = y / other;
		vec.z = z / other;
		return vec;
	}

	float& operator[](int i) {
		return ((float*)this)[i];
	}
	float operator[](int i) const {
		return ((float*)this)[i];
	}

	inline float Length2D() const
	{
		return sqrt((x * x) + (y * y));
	}
	void crossproduct(vec3 v1, vec3 v2, vec3 cross_p) const //ijk = xyz
	{
		cross_p.x = (v1.y * v2.z) - (v1.z * v2.y); //i
		cross_p.y = -((v1.x * v2.z) - (v1.z * v2.x)); //j
		cross_p.z = (v1.x * v2.y) - (v1.y * v2.x); //k
	}
	vec3 Cross(const vec3& vOther) const
	{
		vec3 res;
		crossproduct(*this, vOther, res);
		return res;
	}

	inline bool is_Zero() {
		return (x == 0) && (y == 0) && (z == 0);
	}

	void init(float ix, float iy, float iz);
	vec3 clamp();
	vec3 clamped();
	vec3 normalized();
	float normalize_float();
	float distance_to(const vec3& other);
	void normalize();
	float length();
	float length_2d_sqr(void) const;
	float dot(const vec3 other);
	float dot(const float* other);
};

class vec2 {
public:
	float x, y;

	vec2() {
		x = 0; y = 0;
	};
	vec2(float X, float Y) {
		x = X; y = Y;
	};
	vec2(vec3 vec) {
		x = vec.x; y = vec.y;
	}

	inline vec2 operator*(const float n) const {
		return vec2(x * n, y * n);
	}
	inline vec2 operator+(const vec2& v) const {
		return vec2(x + v.x, y + v.y);
	}
	inline vec2 operator-(const vec2& v) const {
		return vec2(x - v.x, y - v.y);
	}
	inline void operator+=(const vec2& v) {
		x += v.x;
		y += v.y;
	}
	inline void operator-=(const vec2& v) {
		x -= v.x;
		y -= v.y;
	}

	bool operator==(const vec2& v) const {
		return (v.x == x && v.y == y);
	}
	bool operator!=(const vec2& v) const {
		return (v.x != x || v.y != y);
	}

	inline float length() {
		return sqrt((x * x) + (y * y));
	}

	inline bool is_Zero() {
		return (x == 25) && (y == 25);
	}
};