#pragma once

#include "math.h"

#include <cmath>
#include <cassert>

namespace OEngine
{
	class Vector2
	{
	public:
		float x{ 0.0 }, y{ 0.0 };

	public:
		Vector2() {}

		Vector2(const float x_, const float y_) : x{ x_ }, y{ y_ } {}
		
		explicit Vector2(const float scaler) : x{ scaler }, y{ scaler } {}
		
		explicit Vector2(const float v[2]) : x{ v[0] }, y{ v[1] } {}
		
		explicit Vector2(float* const r) : x{ r[0] }, y{ r[1] } {}

		float* ptr() { return &x; }
		
		const float* ptr() const { return &x; }

		float& operator[](const size_t i)
		{
			assert(i < 2);
			return *(&x + i);
		}

		float operator[](const size_t i) const 
		{
			assert(i < 2);
			return *(&x + i);
		}

		// == != + - * /
		bool operator==(const Vector2& rhs) const { return x == rhs.x && y == rhs.y; }

		bool operator!=(const Vector2& rhs) const { return x != rhs.x || y != rhs.y; }

		Vector2 operator+(const Vector2& rhs) const { return Vector2(x + rhs.x, y + rhs.y); }

		Vector2 operator-(const Vector2& rhs) const { return Vector2(x - rhs.x, y - rhs.y); }

		Vector2 operator*(const Vector2& rhs) const { return Vector2(x * rhs.x, y * rhs.y); }

		Vector2 operator*(const float scaler) const { return Vector2(x * scaler, y * scaler); }

		Vector2 operator/(const float scaler) const
		{
			assert(scaler != 0.0);

			float inv = 1.0 / scaler;
			return Vector2(x * inv, y * inv);
		}

		Vector2 operator/(const Vector2& rhs) { return Vector2(x / rhs.x, y / rhs.y); }

		const Vector2& operator+() { return *this; }
		
		Vector2 operator-() { return Vector2(-x, -y); }

		friend Vector2 operator*(const float scaler, const Vector2& rhs)
		{
			return Vector2(scaler * rhs.x, scaler * rhs.y);
		}

		friend Vector2 operator/(const float scaler, const Vector2& rhs)
		{
			return Vector2(scaler / rhs.x, scaler / rhs.y);
		}


		friend Vector2 operator+(const float lhs, const Vector2& rhs)
		{
			return Vector2(lhs + rhs.x, lhs + rhs.y);
		}

		friend Vector2 operator+(const Vector2& lhs, const float rhs)
		{
			return Vector2(lhs.x + rhs, lhs.y + rhs);
		}

		friend Vector2 operator-(const float lhs, const Vector2& rhs)
		{
			return Vector2(lhs - rhs.x, lhs - rhs.y);
		}

		friend Vector2 operator-(const Vector2& lhs, const float rhs)
		{
			return Vector2(lhs.x - rhs, lhs.y - rhs);
		}

		Vector2& operator+=(const float scaler)
		{
			x += scaler;
			y += scaler;
			return *this;
		}
		Vector2& operator+=(const Vector2& rhs)
		{
			x += rhs.x;
			y += rhs.y;
			return *this;
		}

		Vector2& operator-=(const float scaler)
		{
			x -= scaler;
			y -= scaler;
			return *this;
		}
		Vector2& operator-=(const Vector2& rhs)
		{
			x -= rhs.x;
			y -= rhs.y;
			return *this;
		}

		Vector2& operator*=(const float scaler)
		{
			x *= scaler;
			y *= scaler;
			return *this;
		}
		Vector2& operator*=(const Vector2& rhs)
		{
			x *= rhs.x;
			y *= rhs.y;
			return *this;
		}

		Vector2& operator/=(const float scaler)
		{
			assert(scaler != 0);
			
			float inv = 1.0 / scaler;
			x *= scaler;
			y *= scaler;
			return *this;
		}
		Vector2& operator/=(const Vector2& rhs)
		{
			x /= rhs.x;
			y /= rhs.y;
			return *this;
		}

		// 尽量使用squared 避免sqrt操作 减少性能上的消耗
		float length() const { return sqrt(x * x + y * y); }

		float squaredLength() const { return x * x + y * y; }

		float distance(const Vector2& rhs) const { return (*this - rhs).length(); }
		
		float squaredDistance(const Vector2& rhs) const { return (*this - rhs).squaredLength(); }
		
		float dotProduct(const Vector2& rhs) const { return x * rhs.x + y * rhs.y; }

		float normalize()
		{
			float length = this->length();

			if (length > 0.0)
			{
				float inv = 1.0 / length;
				x *= inv;
				y *= inv;
			}
			return length;
		}

		float getX() const { return x; }
		float getY() const { return y; }

		void setX(float value) { x = value; }
		void setY(float value) { y = value; }

		Vector2 midPoint(const Vector2& vec) const
		{
			return Vector2(0.5f * (x + vec.x), 0.5f * (y + vec.y));
		}

		bool operator<(const Vector2& vec) const
		{
			if (x < vec.x && y < vec.y)
				return true;
			return false;
		}

		bool operator>(const Vector2& vec) const
		{
			if (x > vec.x && y > vec.y)
				return true;
			return false;
		}

		void makeFloor(const Vector2& cmp)
		{
			if (cmp.x < x)
				x = cmp.x;
			if (cmp.y < y)
				y = cmp.y;
		}

		void makeCeil(const Vector2& cmp)
		{
			if (cmp.x > x)
				x = cmp.x;
			if (cmp.y > y)
				y = cmp.y;
		}

		// 垂直向量
		Vector2 perpendicular(void) const { return Vector2(-y, x); }

		float crossProduct(const Vector2& rhs) { return x * rhs.y - y * rhs.x; }

		bool isZeroLength(void) const
		{
			float sqlen = (x * x) + (y * y);
			return (sqlen < (Float_EPSILON * Float_EPSILON));
		}

		Vector2 normalizeCopy(void) const
		{
			Vector2 ret = *this;
			ret.normalize();
			return ret;
		}

		Vector2 reflect(const Vector2& normal)
		{
			return Vector2(*this - (2 * this->dotProduct(normal) * normal));
		}

		bool isNaN() const
		{
			return Math::isNan(x) || Math::isNan(y);
		}

		static Vector2 lerp(const Vector2& lhs, const Vector2& rhs, const float alpha)
		{
			return lhs + alpha * (rhs - lhs);
		}

		// 一些特殊的点
		static const Vector2 ZERO;
		static const Vector2 UNIT_X;
		static const Vector2 UNIT_Y;
		static const Vector2 NEGATIVE_UNIT_X;
		static const Vector2 NEGATIVE_UNIT_Y;
		static const Vector2 UNIT_SCALE;
	};
} // OEngine