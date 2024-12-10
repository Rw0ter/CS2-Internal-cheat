#pragma once
#include <directxmath.h>

#define PI 3.141592653f
#define Deg2Rad(Degree) ((Degree) * (PI / 180.f))
#define Rad2Deg(Radius) ((Radius) * (180.f / PI))
struct Vector3
{
    float x, y, z;

    // 默认构造函数
    Vector3() : x(0), y(0), z(0) {}

    // 带参数的构造函数
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

    Vector3 operator+(Vector3 d);
    Vector3 operator-(Vector3 d);
    Vector3 operator*(Vector3 d);
    Vector3 operator*(float d);
    Vector3& operator-=(Vector3 d);
    Vector3& operator+=(Vector3 d);

    // Lerp 方法声明
    Vector3 Lerp(const Vector3& target, float t) const;

    Vector3 Normalized() const;
    float Length() const;
    float Length2DSqr() const;
    float Length2D() const;

    Vector3 AnglesToVectors(Vector3* pForward, Vector3* pRight = nullptr, Vector3* pUp = nullptr) const;

};

struct Vector4
{
    float x, y, w, h;

    Vector4 operator+(Vector4 d);
    Vector4 operator-(Vector4 d);
    Vector4 operator*(Vector4 d);
    Vector4 operator*(float d);
};