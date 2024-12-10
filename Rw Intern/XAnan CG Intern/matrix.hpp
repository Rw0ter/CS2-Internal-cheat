#pragma once
#include <bit>
#include <xmmintrin.h>
#include "Vector.h"
#pragma pack(push, 4)
using Matrix3x3 = float[3][3];
class Matrix3x4 {
public:
	Matrix3x4() = default;
	float ArrayData[3][4] = {};
	constexpr Matrix3x4(
		const float ForwardX, const float RightX, const float UpX, const float OriginX,
		const float ForwardY, const float RightY, const float UpY, const float OriginY,
		const float ForwardZ, const float RightZ, const float UpZ, const float OriginZ
	) {
		ArrayData[0][0] = ForwardX;
		ArrayData[0][1] = RightX;
		ArrayData[0][2] = UpX;
		ArrayData[0][3] = OriginX;
		ArrayData[1][0] = ForwardY;
		ArrayData[1][1] = RightY;
		ArrayData[1][2] = UpY;
		ArrayData[1][3] = OriginY;
		ArrayData[2][0] = ForwardZ;
		ArrayData[2][1] = RightZ;
		ArrayData[2][2] = UpZ;
		ArrayData[2][3] = OriginZ;
	}

	inline Matrix3x4 Clone() const noexcept {
		return Matrix3x4{
			this->GetForward(),
			this->GetLeft(),
			this->GetUp(),
			this->GetOrigin()
		};
	}

	constexpr void SetForward(const Vector3& Forward) {
		ArrayData[0][0] = Forward.x;
		ArrayData[1][0] = Forward.y;
		ArrayData[2][0] = Forward.z;
	}

	constexpr void SetRight(const Vector3& Right) {
		ArrayData[0][1] = Right.x;
		ArrayData[1][1] = Right.y;
		ArrayData[2][1] = Right.z;
	}

	constexpr void SetUp(const Vector3& Up) {
		ArrayData[0][2] = Up.x;
		ArrayData[1][2] = Up.y;
		ArrayData[2][2] = Up.z;
	}

	constexpr void SetOrigin(const Vector3& Origin) {
		ArrayData[0][3] = Origin.x;
		ArrayData[1][3] = Origin.y;
		ArrayData[2][3] = Origin.z;
	}

	constexpr Matrix3x4(const Vector3& Forward, const Vector3& Right, const Vector3& Up, const Vector3& Origin) {
		SetUp(Up);
		SetRight(Right);
		SetOrigin(Origin);
		SetForward(Forward);
	}

	[[nodiscard]] float* operator[](const int Index) {
		return ArrayData[Index];
	}

	[[nodiscard]] const float* operator[](const int Index) const {
		return ArrayData[Index];
	}

	Vector3 GetForward() const {
		return Vector3(ArrayData[0][0], ArrayData[1][0], ArrayData[2][0]);
	}

	Vector3 GetLeft() const {
		return Vector3(ArrayData[0][1], ArrayData[1][1], ArrayData[2][1]);
	}

	Vector3 GetUp() const {
		return Vector3(ArrayData[0][2], ArrayData[1][2], ArrayData[2][2]);
	}

	Vector3 GetOrigin() const {
		return Vector3(ArrayData[0][3], ArrayData[1][3], ArrayData[2][3]);
	}

	constexpr void Invalidate() {
		for (auto& ArraySubData : ArrayData) {
			for (auto& flData : ArraySubData) {
				flData = std::numeric_limits<float>::infinity();
			}
		}
	}
};