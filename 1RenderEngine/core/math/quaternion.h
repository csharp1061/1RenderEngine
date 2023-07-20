#pragma once

#include "math.h"
#include <cassert>

/*
*  
*/

namespace OEngine
{
	class Matrix3x3;
	class Vector3;

	class Quaternion
	{
	public:
		float w{ 1.f }, x{ 0.f }, y{ 0.f }, z{ 0.f };

	public:
		Quaternion() {}
		Quaternion(float _w, float _x, float _y, float _z) : w{ _w }, x{ _x }, y{ _y }, z{ _z } {}

        Quaternion(const Matrix3x3& mat) { this->fromRotationMatrix(mat); }
        Quaternion(const Radian& angle, const Vector3& axis) { this->fromAngleAxis(angle, axis); }
        Quaternion(const Vector3& xaxis, const Vector3& yaxis, const Vector3& zaxis)
        {
            this->fromAxes(xaxis, yaxis, zaxis);
        }

		float* ptr() { return &w; }
		const float* ptr() const { return &w; }

		void fromRotationMatrix(const Matrix3x3& rotation);
		void toRotationMatrix(Matrix3x3& rotation) const;

		void fromAngleAxis(const Radian& angle, const Vector3& axis);

		static Quaternion getQuaternionFromAngleAxis(const Radian& angle, const Vector3& axis);

		void fromDirection(const Vector3& direction, const Vector3& up_direction);

		static Quaternion getQuaternionFromDirection(const Vector3& direction, const Vector3& up_direction);

		void toAngleAxis(Radian& angle, Vector3& axis) const;

		// 使用坐标轴构建四元数，坐标轴之间互相垂直
		void fromAxes(const Vector3& x_axis, const Vector3& y_axis, const Vector3& z_axis);
		// 通过四元数获得坐标轴 x, y, z
		void toAxes(Vector3& x_axis, Vector3& y_axis, Vector3& z_axis) const;

		/** Returns the X orthonormal axis defining the quaternion. Same as doing
		   xAxis = Vector3::UNIT_X * this. Also called the local X-axis
	   */
		Vector3 xAxis(void) const;

		/** Returns the Y orthonormal axis defining the quaternion. Same as doing
			yAxis = Vector3::UNIT_Y * this. Also called the local Y-axis
		*/
		Vector3 yAxis(void) const;

		/** Returns the Z orthonormal axis defining the quaternion. Same as doing
			zAxis = Vector3::UNIT_Z * this. Also called the local Z-axis
		*/
		Vector3 zAxis(void) const;

		Quaternion operator+(const Quaternion& rhs) const
		{
			return Quaternion(w + rhs.w, x + rhs.x, y + rhs.y, z + rhs.z);
		}

		Quaternion operator-(const Quaternion& rhs) const
		{
			return Quaternion(w - rhs.w, x - rhs.x, y - rhs.y, z - rhs.z);
		}

        Quaternion mul(const Quaternion& rhs) const { return (*this) * rhs; }
        Quaternion operator*(const Quaternion& rhs) const;

        Quaternion operator*(const float scalar) const
        {
            return Quaternion(w * scalar, x * scalar, y * scalar, z * scalar);
        }

        //// rotation of a vector by a quaternion
        Vector3 operator*(const Vector3& rhs) const;

        Quaternion operator/(const float scalar) const
        {
            assert(scalar != 0.0f);
            return Quaternion(w / scalar, x / scalar, y / scalar, z / scalar);
        }

        friend Quaternion operator*(float scalar, const Quaternion& rhs)
        {
            return Quaternion(scalar * rhs.w, scalar * rhs.x, scalar * rhs.y, scalar * rhs.z);
        }

        Quaternion operator-() const { return Quaternion(-w, -x, -y, -z); }

        bool operator==(const Quaternion& rhs) const
        {
            return (rhs.x == x) && (rhs.y == y) && (rhs.z == z) && (rhs.w == w);
        }

        bool operator!=(const Quaternion& rhs) const
        {
            return (rhs.x != x) || (rhs.y != y) || (rhs.z != z) || (rhs.w != w);
        }

        /// Check whether this quaternion contains valid values
        bool isNaN() const { return Math::isNan(x) || Math::isNan(y) || Math::isNan(z) || Math::isNan(w); }

        float getX() const { return x; }
        float getY() const { return y; }
        float getZ() const { return z; }
        float getW() const { return w; }

        // functions of a quaternion
        float dot(const Quaternion& rkQ) const { return w * rkQ.w + x * rkQ.x + y * rkQ.y + z * rkQ.z; }

        float length() const { return sqrt(w * w + x * x + y * y + z * z); }

        /// Normalizes this quaternion, and returns the previous length
        void normalise(void)
        {
            float len = w * w + x * x + y * y + z * z;
            float factor = 1.0f / sqrt(len);
            *this = *this * factor;
        }

        Quaternion inverse() const // apply to non-zero quaternion
        {
            float norm = w * w + x * x + y * y + z * z;
            if (norm > 0.0)
            {
                float inv_norm = 1.0f / norm;
                return Quaternion(w * inv_norm, -x * inv_norm, -y * inv_norm, -z * inv_norm);
            }
            else
            {
                // return an invalid result to flag the error
                return ZERO;
            }
        }

        Radian getRoll(bool reproject_axis = true) const;
        Radian getPitch(bool reproject_axis = true) const;
        Radian getYaw(bool reproject_axis = true) const;

        static Quaternion sLerp(float t, const Quaternion& kp, const Quaternion& kq, bool shortest_path = false);
        
        static Quaternion nLerp(float t, const Quaternion& kp, const Quaternion& kq, bool shortest_path = false);

        Quaternion conjugate() const { return Quaternion(w, -x, -y, -z); }

        // special values
        static const Quaternion ZERO;
        static const Quaternion IDENTITY;

        static const float k_epsilon;
	};
} // OEngine