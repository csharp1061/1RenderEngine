#pragma once

#include "vector3.h"
#include "math.h"

namespace OEngine
{
	class Vector3;

	class Vector4
	{
	public:
		float x{0.f}, y{0.f}, z{0.f}, w{0.f};

	public:
		Vector4() {}
		Vector4(float scaler) : x{ scaler }, y{ scaler }, z{ scaler }, w{ scaler } {}
		Vector4(float _x, float _y, float _z, float _w) : x{ _x }, y{ _y }, z{ _z }, w{ _w } {}
		Vector4(const Vector3& vec, float _w = 1.f) : x{ vec.x }, y{ vec.y }, z{ vec.z }, w{ _w } {}

		explicit Vector4(float coords[4]) : x{ coords[0] }, y{ coords[1] }, z{ coords[2] }, w{ coords[3] } {}

		float operator[](const size_t i) const
		{
			assert(i < 4);
			return *(&x + i);
		}

		float& operator[](const size_t i)
		{
			assert(i < 4);
			return *(&x + i);
		}

		float* ptr() { return &x; }
		const float* ptr() const { return &x; }

		Vector4& operator=(float scaler)
		{
			x = scaler;
			y = scaler;
			z = scaler;
			w = scaler;
			return *this;
		}

		bool operator==(const Vector4& rhs) const { return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w; }
		
		bool operator!=(const Vector4& rhs) const { return !(*this == rhs); }
	
		Vector4 operator+(const Vector4& rhs) const { return Vector4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w); }

		Vector4 operator-(const Vector4& rhs) const { return Vector4(x - rhs.x, y + rhs.y, z + rhs.z, w + rhs.w); }

		Vector4 operator*(const Vector4& rhs) const { return Vector4(x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w); }

		Vector4 operator*(float scaler) const { return Vector4(x * scaler, y * scaler, z * scaler, w * scaler); }

		Vector4 operator/(const Vector4& rhs) const 
		{ 
			assert(rhs.x != 0.f && rhs.y != 0.f && rhs.z != 0.f && rhs.w != 0.f);
			return Vector4(x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.w);
		}

		Vector4 operator/(float scaler) const
		{
			assert(scaler != 0.f);
			return Vector4(x / scaler, y / scaler, z / scaler, w / scaler);
		}

		const Vector4& operator+() const { return *this; }

		Vector4 operator-() const { return Vector4(-x, -y, -z, -w); }

		// friend --const float?
		friend Vector4 operator*(const float scaler, const Vector4& rhs)
		{
			return Vector4(scaler * rhs.x, scaler * rhs.y, scaler * rhs.z, scaler * rhs.w);
		}

		friend Vector4 operator/(const float scaler, const Vector4& rhs)
		{
			assert(rhs.x != 0.f && rhs.y != 0.f && rhs.z != 0.f && rhs.w != 0.f);
			return Vector4(scaler / rhs.x, scaler / rhs.y, scaler / rhs.z, scaler / rhs.w);
		}

		friend Vector4 operator+(const float lhs, const Vector4& rhs)
		{
			return Vector4(lhs + rhs.x, lhs + rhs.y, lhs + rhs.z, lhs + rhs.w);
		}

		friend Vector4 operator+(const Vector4& lhs, const float rhs)
		{
			return Vector4(lhs.x + rhs, lhs.y + rhs, lhs.z + rhs, lhs.w + rhs);
		}

		friend Vector4 operator-(const float lhs, const Vector4& rhs)
		{
			return Vector4(lhs + rhs.x, lhs + rhs.y, lhs + rhs.z, lhs + rhs.w);
		}

		friend Vector4 operator-(const Vector4& lhs, const float rhs)
		{
			return Vector4(lhs.x + rhs, lhs.y + rhs, lhs.z + rhs, lhs.w + rhs);
		}

		// 二元运算符
		Vector4& operator+=(const Vector4& rhs)
		{
			x += rhs.x;
			y += rhs.y;
			z += rhs.z;
			w += rhs.w;
			return *this;
		}

		Vector4& operator+=(const float scaler)
		{
			x += scaler;
			y += scaler;
			z += scaler;
			w += scaler;
			return *this;
		}

		Vector4& operator-=(const Vector4& rhs)
		{
			x -= rhs.x;
			y -= rhs.y;
			z -= rhs.z;
			w -= rhs.w;
			return *this;
		}

		Vector4& operator-=(const float scaler)
		{
			x -= scaler;
			y -= scaler;
			z -= scaler;
			w -= scaler;
			return *this;
		}

		Vector4& operator*=(const Vector4& rhs)
		{
			x *= rhs.x;
			y *= rhs.y;
			z *= rhs.z;
			w *= rhs.w;
			return *this;
		}

		Vector4& operator*=(const float scaler)
		{
			x *= scaler;
			y *= scaler;
			z *= scaler;
			w *= scaler;
			return *this;
		}

		Vector4& operator/=(const Vector4& rhs)
		{
			assert(rhs.x != 0.f && rhs.y != 0.f && rhs.z != 0.f && rhs.w != 0.f);
			x /= rhs.x;
			y /= rhs.y;
			z /= rhs.z;
			w /= rhs.w;
			return *this;
		}

		Vector4& operator/=(const float scaler)
		{
			assert(scaler != 0.f);
			x /= scaler;
			y /= scaler;
			z /= scaler;
			w /= scaler;
			return *this;
		}

		// dot
		float dotProduct(const Vector4& rhs) { return x * rhs.x + y * rhs.y + z * rhs.z + w * rhs.w; }

		Vector3 to_vec3() { return Vector3(x, y, z); }
		// valid 合法性检测
		bool isNaN() { return Math::isNan(x) || Math::isNan(y) || Math::isNan(z) || Math::isNan(w); }

		static Vector4 lerp(const Vector4& lhs, const Vector4& rhs, float alpha)
		{
			return lhs + alpha * (rhs - lhs);
		}

		static const Vector4 ZERO;
		static const Vector4 UNIT_SCALE;
	};
} // OEngine