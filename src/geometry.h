#pragma once

#ifdef USE_FASTLED
#include <FastLED.h>
#endif

#include "colortypes.h"

namespace LightString {

struct Rect {
	int16_t x = 0, y = 0, width = 0, height = 0;

	Rect() {}

	Rect(const int16_t x, const int16_t y, const int16_t width, const int16_t height)
		: x(x), y(y), width(width), height(height) {}
};

template <typename T>
struct Vector {
	T x, y;

	Vector(const T x = 0, const T y = 0) : x(x), y(y) {}

	inline Vector &operator+=(const Vector<T> &rhs) __attribute__((always_inline))
	{
		this->x += rhs.x;
		this->y += rhs.y;

		return *this;
	}

	inline Vector &operator-=(const Vector<T> &rhs) __attribute__((always_inline))
	{
		this->x -= rhs.x;
		this->y -= rhs.y;

		return *this;
	}

	inline Vector &operator*=(const T &rhs) __attribute__((always_inline))
	{
		this->x *= rhs;
		this->y *= rhs;

		return *this;
	}

	inline Vector &operator/=(const T &rhs) __attribute__((always_inline))
	{
		this->x /= rhs;
		this->y /= rhs;

		return *this;
	}
	
	void print() {
#ifdef ARDUINO
		Serial.print("(");
		Serial.print(x);
		Serial.print(", ");
		Serial.print(y);
		Serial.print(")");
#else
		printf("(%f, %f)", x, y);
#endif
	}
};

template <typename T>
struct Vector3d {
	T x, y, z;

	Vector3d(const T x = 0, const T y = 0, const T z = 0) : x(x), y(y), z(z) {}

	inline Vector3d &operator+=(const Vector3d<T> &rhs) __attribute__((always_inline))
	{
		this->x += rhs.x;
		this->y += rhs.y;
		this->z += rhs.z;

		return *this;
	}

	inline Vector3d &operator-=(const Vector3d<T> &rhs) __attribute__((always_inline))
	{
		this->x -= rhs.x;
		this->y -= rhs.y;
		this->z -= rhs.z;

		return *this;
	}

	inline Vector3d &operator*=(const T &rhs) __attribute__((always_inline))
	{
		this->x *= rhs;
		this->y *= rhs;
		this->z *= rhs;

		return *this;
	}

	inline Vector3d &operator/=(const T &rhs) __attribute__((always_inline))
	{
		this->x /= rhs;
		this->y /= rhs;
		this->z /= rhs;

		return *this;
	}
	
	void print() {
#ifdef ARDUINO
		Serial.print("(");
		Serial.print(x);
		Serial.print(", ");
		Serial.print(y);
		Serial.print(")");
#else
		printf("(%f, %f, %f)", x, y, z);
#endif
	}
};

template <typename T>
struct Point {
	T x, y;

	Point(const T x = 0, const T y = 0) : x(x), y(y) {}

	inline Point &operator+=(const Point<T> &rhs) __attribute__((always_inline))
	{
		this->x += rhs.x;
		this->y += rhs.y;

		return *this;
	}

	inline Point &operator+=(const Vector<T> &rhs) __attribute__((always_inline))
	{
		this->x += rhs.x;
		this->y += rhs.y;

		return *this;
	}

	inline Point &operator-=(const Point<T> &rhs) __attribute__((always_inline))
	{
		this->x -= rhs.x;
		this->y -= rhs.y;

		return *this;
	}

	inline Point &operator-=(const Vector<T> &rhs) __attribute__((always_inline))
	{
		this->x -= rhs.x;
		this->y -= rhs.y;

		return *this;
	}
	
	void print() {
#ifdef ARDUINO
		Serial.print("(");
		Serial.print(x);
		Serial.print(", ");
		Serial.print(y);
		Serial.print(")");
#else
		printf("(%f, %f)", x, y);
#endif
	}
};

template <typename T>
struct Point3d {
	T x = 0, y = 0, z = 0;

	Point3d(const T x = 0, const T y = 0, const T z = 0) : x(x), y(y), z(z) {}

	inline Point3d &operator+=(const Point3d<T> &rhs) __attribute__((always_inline))
	{
		this->x += rhs.x;
		this->y += rhs.y;
		this->z += rhs.z;

		return *this;
	}

	inline Point3d &operator+=(const Vector3d<T> &rhs) __attribute__((always_inline))
	{
		this->x += rhs.x;
		this->y += rhs.y;
		this->z += rhs.z;

		return *this;
	}

	inline Point3d &operator-=(const Point3d<T> &rhs) __attribute__((always_inline))
	{
		this->x -= rhs.x;
		this->y -= rhs.y;
		this->z -= rhs.z;

		return *this;
	}

	inline Point3d &operator-=(const Vector3d<T> &rhs) __attribute__((always_inline))
	{
		this->x -= rhs.x;
		this->y -= rhs.y;
		this->z -= rhs.z;

		return *this;
	}
	
	void print() {
#ifdef ARDUINO
		Serial.print("(");
		Serial.print(x);
		Serial.print(", ");
		Serial.print(y);
		Serial.print(")");
#else
		printf("(%f, %f, %f)", x, y, z);
#endif
	}
};
/*
struct MovingPoint {
	Point<float> origin;
	Vector<float> vel;
	Vector<float> acc;
	float friction;

	MovingPoint() : friction(0) {}
	MovingPoint(float x, float y, float vx = 0, float vy = 0, float ax = 0, float ay = 0, float friction = 0) {
		origin = Point<float>(x, y);
		vel = Vector<float>(vx, vy);
		acc = Vector<float>(ax, ay);
		this->friction = friction;
	}

	inline void update() {
		origin += vel;
		vel += acc;

		if (friction != 0) {
			vel *= 1.0 - friction;
			acc *= 1.0 - friction;
		}
	}
};*/

template<template <typename> class PT, template <typename> class VEC>
struct MovingPoint {
	PT<float> origin;
	VEC<float> vel;
	VEC<float> acc;
	float friction;

	MovingPoint() : friction(0) {}
	MovingPoint(PT<float> origin, VEC<float> vel, VEC<float> acc, float friction = 0) :
		origin(origin), vel(vel), acc(acc), friction(friction) {}

	inline void update() {
		origin += vel;
		vel += acc;

		if (friction != 0) {
			vel *= 1.0 - friction;
			acc *= 1.0 - friction;
		}
	}
};

};
