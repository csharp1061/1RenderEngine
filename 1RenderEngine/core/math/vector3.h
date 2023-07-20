#pragma once

#include "math.h"

#include <cassert>

namespace OEngine {
	class Vector4;

	class Vector3
	{
	public:
		float x{ 0.0 };
		float y{ 0.0 };
		float z{ 0.0 };

	public:
		Vector3() {}
		Vector3(float scaler) : x{ scaler }, y{ scaler }, z{ scaler } {}
		Vector3(float _x, float _y, float _z) : x{ _x }, y{ _y }, z{ _z } {}

		explicit Vector3(const float coord[3]) : x{ coord[0] }, y{ coord[1] }, z{ coord[2] } {}

		// ����ֵ ȡֵ ����ı�ԭֵ
		float operator[](const size_t i) const
		{
			assert(i < 3);
			return *(&x + i);
		}

		// ���Ըı�ԭֵ
		float& operator[](const size_t i)
		{
			assert(i < 3);
			return *(&x + i);
		}

		float* ptr() { return &x; }
		const float* ptr() const { return &x; }

		bool operator==(const Vector3& rhs) const { return x == rhs.x && y == rhs.y && z == rhs.z; }

		bool operator!=(const Vector3& rhs) const { return x != rhs.x || y != rhs.y || z != rhs.z; }

		// �������������
		Vector3 operator+(const Vector3& rhs) const { return Vector3(x + rhs.x, y + rhs.y, z + rhs.z); }

		Vector3 operator-(const Vector3& rhs) const { return Vector3(x - rhs.x, y - rhs.y, z - rhs.z); }

		Vector3 operator*(const Vector3& rhs) const { return Vector3(x * rhs.x, y * rhs.y, z * rhs.z); }

		Vector3 operator*(const float scaler) const { return Vector3(x * scaler, y * scaler, z * scaler); }

		Vector3 operator/(const Vector3& rhs) const 
		{ 
			assert(rhs.x != 0.0 && rhs.y != 0.0 && rhs.z != 0.0);
			return Vector3(x / rhs.x, y / rhs.y, z / rhs.y); 
		}

		Vector3 operator/(const float scaler) const
		{
			assert(scaler != 0.0);

			float inv = 1.0 / scaler;
			return Vector3(x * inv, y * inv, z * inv);
		}

		const Vector3& operator+() const { return *this; }

		Vector3 operator-() const { return Vector3(-x, -y, -z); }

		friend Vector3 operator*(const float scaler, const Vector3& rhs)
		{
			return Vector3(scaler * rhs.x, scaler * rhs.y, scaler * rhs.z);
		}
		
		friend Vector3 operator/(const float scaler, const Vector3& rhs)
		{
			assert(rhs.x != 0.0 && rhs.y != 0.0 && rhs.z != 0.0);
			return Vector3(scaler / rhs.x, scaler / rhs.y, scaler / rhs.z);
		}

		// lhs & rhs
		friend Vector3 operator+(const float lhs, const Vector3& rhs)
		{
			return Vector3(lhs + rhs.x, lhs + rhs.y, lhs + rhs.z);
		}

		friend Vector3 operator+(const Vector3& lhs, const float rhs)
		{
			return Vector3(lhs.x + rhs, lhs.y + rhs, lhs.z + rhs);
		}

		friend Vector3 operator-(const float lhs, const Vector3& rhs)
		{
			return Vector3(lhs - rhs.x, lhs - rhs.y, lhs - rhs.z);
		}

		friend Vector3 operator-(const Vector3& lhs, const float rhs)
		{
			return Vector3(lhs.x - rhs, lhs.y - rhs, lhs.z - rhs);
		}

		// ��Ԫ�����
		Vector3& operator+=(const float scaler)
		{
			x += scaler;
			y += scaler;
			z += scaler;
			return *this;
		}

		Vector3& operator+=(const Vector3& rhs)
		{
			x += rhs.x;
			y += rhs.y;
			z += rhs.z;
			return *this;
		}

		Vector3& operator-=(const float scaler)
		{
			x -= scaler;
			y -= scaler;
			z -= scaler;
			return *this;
		}

		Vector3& operator-=(const Vector3& rhs)
		{
			x -= rhs.x;
			y -= rhs.y;
			z -= rhs.z;
			return *this;
		}

		Vector3& operator*=(const float scaler)
		{
			x *= scaler;
			y *= scaler;
			z *= scaler;
			return *this;
		}

		Vector3& operator*=(const Vector3& rhs)
		{
			x *= rhs.x;
			y *= rhs.y;
			z *= rhs.z;
			return *this;
		}

		Vector3& operator/=(const float scaler)
		{
			assert(scaler != 0);
			x /= scaler;
			y /= scaler;
			z /= scaler;
			return *this;
		}

		Vector3& operator/=(const Vector3& rhs)
		{
			assert(rhs.x != 0 && rhs.y != 0 && rhs.z != 0);
			x /= rhs.x;
			y /= rhs.y;
			z /= rhs.z;
			return *this;
		}

		// ��������ʹ��length����ʹ��squaredLength���� ���������ϵ�����
		float length() const { return sqrt(x * x + y * y + z * z); }

		float squaredLength() const { return x * x + y * y + z * z; }

		float distance(const Vector3& rhs) const { return (*this - rhs).length(); }

		float squaredLength(const Vector3& rhs) const { return (*this - rhs).squaredLength(); }

		// ��� �ɱ�ʾ�Ƕ� ��ض�  ע��õ�˲������������ĵ�λ��
		float dotProduct(const Vector3& rhs) const { return x * rhs.x + y * rhs.y + z * rhs.z; }

		// ��λ������  �����������������κβ���
		void normalise()
		{
			float length = sqrt(x * x + y * y + z * z);
			if (length == 0.0f)
				return;

			float inv = 1.0f / length;
			x *= inv;
			y *= inv;
			z *= inv;
		}

		Vector3 crossProduct(const Vector3& rhs) const
		{
			return Vector3(y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x);
		}


		// ������ֵ��
		void makeFloor(const Vector3& cmp)
		{
			if (cmp.x < x)
				x = cmp.x;
			if (cmp.y < y)
				y = cmp.y;
			if (cmp.z < z)
				z = cmp.z;
		}

		void makeCeil(const Vector3& cmp)
		{
			if (cmp.x > x)
				x = cmp.x;
			if (cmp.y > y)
				y = cmp.y;
			if (cmp.z > z)
				z = cmp.z;
		}

		// ��ʾ���������
		Radian angelBetween(const Vector3& dest)
		{
			float dest_length = length() * dest.length();

			if (dest_length < 1e-6f)
				dest_length = 1e-6f;

			float f = dotProduct(dest) / dest_length;
			f = Math::clamp(f, (float) - 1.0f, (float) 1.0f);
			return Math::acos(f);
		}

		bool isZeroLength(void) const
		{
			float sqlen = x * x + y * y + z * z;
			return (sqlen < (1e-06f * 1e-06f));
		}

		bool isZero(void) const { return x == 0.f && y == 0.f && z == 0.f; }

		Vector3 normalizedCopy(void) const
		{
			Vector3 ret = *this;
			ret.normalise();
			return ret;
		}

		// this Ϊ���䷽��  ͨ��normal���㷴�䷽��
		Vector3 reflect(const Vector3& normal)
		{
			return Vector3(*this - (2 * this->dotProduct(normal) * normal));
		}

		// ����Ͷ�䵽ƽ���ϸ�������
		Vector3 project(const Vector3& normal)
		{
			return Vector3(*this - (2 * this->dotProduct(normal) * normal));
		}

		// ���������ľ���ֵ�����µ�����
		Vector3 absoluteCopy() const
		{
			return Vector3(abs(x), abs(y), abs(z));
		}

		// ���Բ�ֵ
		static Vector3 lerp(const Vector3& lhs, const Vector3& rhs, float alpha)
		{
			return lhs + alpha * (rhs - lhs);
		}

		// ��Χ�޶�
		static Vector3 clamp(const Vector3& v, const Vector3& min, const Vector3& max)
		{
			return Vector3(Math::clamp(v.x, min.x, max.x), Math::clamp(v.y, min.y, max.y), Math::clamp(v.z, min.z, max.z));
		}

		static float getMaxElement(const Vector3& v)
		{
			return Math::getMaxElement(v.x, v.y, v.z);
		}

		bool isNaN() const
		{
			return Math::isNan(x) || Math::isNan(y) || Math::isNan(z);
		}

		static const Vector3 ZERO;
		static const Vector3 UNIT_X;
		static const Vector3 UNIT_Y;
		static const Vector3 UNIT_Z;
		static const Vector3 NEGATIVE_UNIT_X;
		static const Vector3 NEGATIVE_UNIT_Y;
		static const Vector3 NEGATIVE_UNIT_Z;
		static const Vector3 UNIT_SCALE;
	};

} // OEngine