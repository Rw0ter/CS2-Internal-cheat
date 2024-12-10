#include <cmath> 
#include "Vector.h"

Vector3 Vector3::operator+(Vector3 d)
{
    return { x + d.x, y + d.y, z + d.z };
}

Vector3 Vector3::operator-(Vector3 d)
{
    return { x - d.x, y - d.y, z - d.z };
}

Vector3 Vector3::operator*(Vector3 d)
{
    return { x * d.x, y * d.y, z * d.z };
}

Vector3 Vector3::operator*(float d)
{
    return { x * d, y * d, z * d };
}

Vector3& Vector3::operator-=(Vector3 d) {
    Vector3 vecRes = *this - d;
    *this = vecRes;
    return *this;
}

Vector3& Vector3::operator+=(Vector3 d) {
    Vector3 vecRes = *this + d;
    *this = vecRes;
    return *this;
}

Vector4 Vector4::operator+(Vector4 d)
{
    return { x + d.x, y + d.y, w + d.w, h + d.h };
}

Vector4 Vector4::operator-(Vector4 d)
{
    return { x - d.x, y - d.y, w - d.w, h - d.h };
}

Vector4 Vector4::operator*(Vector4 d)
{
    return { x * d.x, y * d.y, w * d.w, h * d.h };
}

Vector4 Vector4::operator*(float d)
{
    return { x * d, y * d, w * d, h * d};
}

// 计算向量的长度
float Vector3::Length() const {
    return std::sqrt(x * x + y * y + z * z);
}

float Vector3::Length2DSqr() const {
    return (x * x + y * y);
}

float Vector3::Length2D() const {
    return std::sqrtf(Length2DSqr());
}

// 归一化向量
Vector3 Vector3::Normalized() const {
    float length = Length();
    if (length == 0) return Vector3(0, 0, 0); // 避免除以零
    return Vector3(x / length, y / length, z / length);
}


Vector3 Vector3::Lerp(const Vector3& target, float t) const
{
    return Vector3(
        this->x + (target.x - this->x) * t,
        this->y + (target.y - this->y) * t,
        this->z + (target.z - this->z) * t
    );
}

Vector3 Vector3::AnglesToVectors(Vector3* pForward, Vector3* pRight, Vector3* pUp) const {
    float flPitchSin, flPitchCos, flYawSin, flYawCos, flRollSin, flRollCos = 0.f;
    DirectX::XMScalarSinCos(&flPitchSin, &flPitchCos, Deg2Rad(this->x));
    DirectX::XMScalarSinCos(&flYawSin, &flYawCos, Deg2Rad(this->y));
    DirectX::XMScalarSinCos(&flRollSin, &flRollCos, Deg2Rad(this->z));
    if (pForward) {
        pForward->x = flPitchCos * flYawCos;
        pForward->y = flPitchCos * flYawSin;
        pForward->z = -flPitchSin;
    }

    if (pRight) {
        pRight->x = (-flRollSin * flPitchSin * flYawCos) + (-flRollCos * -flYawSin);
        pRight->y = (-flRollSin * flPitchSin * flYawSin) + (-flRollCos * flYawCos);
        pRight->z = (-flRollSin * flPitchCos);
    }

    if (pUp) {
        pUp->x = (flRollCos * flPitchSin * flYawCos) + (-flRollSin * -flYawSin);
        pUp->y = (flRollCos * flPitchSin * flYawSin) + (-flRollSin * flYawCos);
        pUp->z = (flRollCos * flPitchCos);
    }

    Vector3* Direction = pForward ? pForward : (pRight ? pRight : pUp);
    return { Direction ? Direction->x : 0.f, Direction ? Direction->y : 0.f, Direction ? Direction->z : 0.f };
}