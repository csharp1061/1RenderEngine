#pragma once

/*
    ��ѧ�����base
*/

#include <cmath>
#include <limits>
#include <stdint.h>

// float���ͱȽ�
#define CMP(x, y) (fabsf(x - y) < FLT_EPSILON * fmaxf(1.0f, fmaxf(fabsf(x), fabsf(y))))

namespace OEngine {
    //global const value
    static const float Math_POS_INFINITY = std::numeric_limits<float>::infinity();
    static const float Math_NEG_INFINITY = -std::numeric_limits<float>::infinity();
    static const float Math_PI = 3.14159265358979323846264338327950288f;

    // 2* pi  1/2 pi   1/pi
    static const float Math_ONE_OVER_PI = 1.0f / Math_PI;
    static const float Math_TWO_PI = 2.0f * Math_PI;
    static const float Math_HALF_PI = 0.5f * Math_PI;

    // ת������
    static const float Math_fDeg2Rad = Math_PI / 180.0f; 
    static const float Math_fRad2Deg = 180.0f / Math_PI;

    static const float Float_EPSILON = FLT_EPSILON;
    static const float Double_EPSILON = DBL_EPSILON;

    // ǰ������
    class Degree; // �Ƕ�
    class Radian; // ����

    class Angle; // �ṩ�ڻ��ȽǶ�֮�������ת���ĽǶȱ�ʾ

    class Vector2;
    class Vector3;
    class Vector4;
    class Matrix3x3;
    class Matrix4x4;
    class Quaternion;
    /*
    * ������
    *
    */
    class Radian
    {
        float m_rad;

    public:
        explicit Radian(float r = 0) : m_rad(r) {}
        Radian(const Degree&);
        Radian(const Radian& r) : m_rad(r.m_rad) {}
        Radian& operator= (const Degree&);
        Radian& operator= (const float& f) 
        {
            m_rad = f;
            return *this;
        }
        
        float valueRadians() const { return m_rad; }
        float valueDegrees() const;
        float valueAngleUnits() const;

        void setValue(const float& f) { m_rad = f; }
        
        const Radian& operator+() const { return *this; }
        Radian operator+(const Radian& r) const { return Radian(m_rad + r.m_rad); }
        Radian operator+(const Degree& d) const;
        Radian& operator+=(const Radian& r) 
        {
            m_rad += r.m_rad;
            return *this;
        }
        Radian& operator+=(const Degree& d);
        
        Radian operator-() const { return Radian(-(this->m_rad)); }
        Radian operator-(const Radian& r) const { return Radian(m_rad - r.m_rad); }
        Radian operator-(const Degree& d) const;
        Radian& operator-=(const Radian& r) 
        {
            m_rad -= r.m_rad;
            return *this;
        }
        Radian& operator-=(const Degree& d);

        Radian operator*(const Radian& r) const { return Radian(m_rad * r.m_rad); }
        Radian operator*(float f) const { return Radian(m_rad * f); }
        Radian& operator*=(float f)
        {
            m_rad *= f;
            return *this;
        }
        Radian operator/(float f) const { return Radian(m_rad / f); }
        Radian& operator/=(float f)
        {
            m_rad /= f;
            return *this;
        }

        // �Ƚ������
        bool operator<(const Radian& r) const { return m_rad < r.m_rad; }
        bool operator>(const Radian& r) const { return m_rad > r.m_rad; }
        bool operator<=(const Radian& r) const { return m_rad <= r.m_rad; }
        bool operator>=(const Radian& r) const { return m_rad >= r.m_rad; }
        bool operator==(const Radian& r) const { return m_rad == r.m_rad; }
        bool operator!=(const Radian& r) const { return m_rad != r.m_rad; }
    };

    /**
    *  represent an angle in degree
    */
    class Degree
    {
        float m_deg;

    public:
        explicit Degree(float f = 0) : m_deg(f) {}
        Degree(const Radian& r) : m_deg(r.valueDegrees()) {}
        Degree& operator=(const Degree& d)
        {
            m_deg = d.m_deg;
            return *this;
        }
        Degree& operator=(const Radian& r)
        {
            m_deg = r.valueDegrees();
            return *this;
        }
        Degree& operator=(float f)
        {
            m_deg = f;
            return *this;
        }

        float valueDegrees() const { return m_deg; }
        float valueRadians() const;
        float valueAngleUnits() const;
        
        // +
        const Degree& operator+() const { return *this; }
        Degree operator+(const Degree& d) const { return Degree(m_deg + d.m_deg); }
        Degree operator+(const Radian& r) const { return Degree(m_deg + r.valueDegrees()); }
        Degree& operator+=(const Degree& d)
        {
            m_deg += d.m_deg;
            return *this;
        }
        Degree& operator+=(const Radian& r)
        {
            m_deg += r.valueDegrees();
            return *this;
        }

        // -
        Degree operator-() const { return Degree(-m_deg); }
        Degree operator-(const Degree& d) const { return Degree(m_deg - d.m_deg); }
        Degree operator-(const Radian& r) const { return Degree(m_deg - r.valueDegrees()); }
        Degree& operator-=(const Degree& d)
        {
            m_deg -= d.m_deg;
            return *this;
        }
        Degree& operator-=(const Radian& r)
        {
            m_deg -= r.valueDegrees();
            return *this;
        }

        // *
        Degree operator*(float f) const { return Degree(m_deg * f); }
        Degree operator*(const Degree& d) const { return Degree(m_deg * d.m_deg); }
        Degree& operator*=(float f)
        {
            m_deg *= f;
            return *this;
        }

        // /
        Degree operator/(float f) const { return Degree(m_deg / f); }
        Degree& operator/=(float f)
        {
            m_deg /= f;
            return *this;
        }

        // �Ƚ������
        bool operator<(const Degree& r) const { return m_deg < r.m_deg; }
        bool operator>(const Degree& r) const { return m_deg > r.m_deg; }
        bool operator<=(const Degree& r) const { return m_deg <= r.m_deg; }
        bool operator>=(const Degree& r) const { return m_deg >= r.m_deg; }
        bool operator==(const Degree& r) const { return m_deg == r.m_deg; }
        bool operator!=(const Degree& r) const { return m_deg != r.m_deg; }
    };

    // �Զ��ڻ��ȺͽǶ�֮��ת��
    class Angle
    {
        float m_angle;

    public:
        Angle(float angle) : m_angle(angle) {}
        Angle() { m_angle = 0; }

        operator Radian() const;
        operator Degree() const;
    };


    class Math
    {
    private:
        enum class AngleUnit
        {
            AU_DEGREE,
            AU_RADIAN
        };
        static AngleUnit k_AngleUnit;

    public:
        Math();

        static float abs(float value) { return float(fabs(value)); }
        static bool isNan(float f) { return std::isnan<float>(f); }
        static float sqr(float value) { return value * value; }
        static float sqrt(float value) { return (float)::sqrt(value); }
        static float invSqrt(float value) { return 1.0f / sqrt(value); }
        static bool realEqual(float a, float b, float tolerance = std::numeric_limits<float>::epsilon());
        static float clamp(float v, float min, float max);
        static float getMaxElement(float a, float b, float c);

        static float degreesToRadians(float degree);
        static float radiansToDegrees(float radian);
        static float angleUnitsToRadians(float units);
        static float radiansToAngleUnits(float radian);
        static float angleUnitsToDegrees(float units);
        static float degreesToAngleUnits(float degree);

        static float sin(const Radian& r) { return ::sin(r.valueRadians()); }
        static float sin(float f) { return ::sin(f); }
        static float cos(const Radian& r) { return ::cos(r.valueRadians()); }
        static float cos(float f) { return ::cos(f); }
        static float tan(const Radian& r) { return ::tan(r.valueRadians()); }
        static float tan(float f) { return ::tan(f); }
        static Radian acos(float value);
        static Radian asin(float value);
        static Radian atan(float value) { return Radian(::atan(value)); }
        static Radian atan2(float y_v, float x_v) { return Radian(::atan2(y_v, x_v)); }

        template <class T>
        static constexpr T max(const T A, const T B)
        {
            return A >= B ? A : B;
        }

        template <class T>
        static constexpr T min(const T A, const T B)
        {
            return A <= B ? A : B;
        }

        template <class T>
        static constexpr T max3(const T A, const T B, const T C)
        {
            return max(max(A, B), C);
        }

        template <class T>
        static constexpr T min3(const T A, const T B, const T C)
        {
            return min(min(A, B), C);
        } 

        static Matrix4x4
            makeViewMatrix(const Vector3& position, const Quaternion& orientation, const Matrix4x4* reflect_matrix = 0);

        static Matrix4x4 makeLookAtMatrix(const Vector3& eye_position, const Vector3& target_position, const Vector3& up_dir);

        static Matrix4x4 makePerspectiveMatrix(Radian fovy, float aspect, float znear, float zfar);

        static Matrix4x4 makeOrthographicProjectionMatrix(const float left,
            const float right,
            const float bottom,
            const float top,
            const float znear,
            const float zfar);
    };
    inline Radian::Radian(const Degree& d) : m_rad(d.valueRadians()) {}
    inline Radian& Radian::operator=(const Degree& d)
    {
        m_rad = d.valueRadians();
        return *this;
    }
    inline Radian Radian::operator+(const Degree& d) const { return Radian(m_rad + d.valueRadians()); }
    inline Radian& Radian::operator+=(const Degree& d)
    {
        m_rad += d.valueRadians();
        return *this;
    }
    inline Radian Radian::operator-(const Degree& d) const { return Radian(m_rad - d.valueRadians()); }
    inline Radian& Radian::operator-=(const Degree& d)
    {
        m_rad -= d.valueRadians();
        return *this;
    }

    inline float Radian::valueDegrees() const { return Math::radiansToDegrees(m_rad); }
    inline float Radian::valueAngleUnits() const { return Math::radiansToAngleUnits(m_rad); }
    inline float Degree::valueRadians() const { return Math::degreesToRadians(m_deg); }
    inline float Degree::valueAngleUnits() const { return Math::degreesToAngleUnits(m_deg); }

    inline Angle::operator Radian() const { return Radian(Math::angleUnitsToRadians(m_angle)); }
    inline Angle::operator Degree() const { return Degree(Math::angleUnitsToDegrees(m_angle)); }

    inline Radian operator*(float a, const Radian& b) { return Radian(a * b.valueRadians()); }

    inline Radian operator/(float a, const Radian& b) { return Radian(a / b.valueRadians()); }

    inline Degree operator*(float a, const Degree& b) { return Degree(a * b.valueDegrees()); }

    inline Degree operator/(float a, const Degree& b) { return Degree(a / b.valueDegrees()); }

} // OEngine