#include "color.hpp"
#include "vector.h"
#include <math.h>
uint8_t FloatToUint(const float Value) {
	return (uint8_t)((int)Value);
}

float UintToFloat(const uint8_t Value) {
	return (float)((int)Value);
}

Color::Color() {
	this->r = 255;
	this->g = 255;
	this->b = 255;
	this->a = 255;
}

void Color::Override(const Color& Other) {
	this->r = Other.r;
	this->g = Other.g;
	this->b = Other.b;
	this->a = Other.a;
}

float* Color::ToFloat() const {
	float Floaters[4] = { UintToFloat(this->r) / 255.f, UintToFloat(this->g) / 255.f, UintToFloat(this->b) / 255.f, UintToFloat(this->a) / 255.f };
	return Floaters;
}

Color::Color(eDefaultColor Default) {
	switch (Default) {
		case eDefaultColor::cWhite:
			*this = Color(255, 255, 255, 255);
		break;
		case eDefaultColor::cBlack:
			*this = Color(0, 0, 0, 255);
		break;
		case eDefaultColor::cBlue:
			*this = Color(0, 0, 255, 255);
		break;
		case eDefaultColor::cGreen:
			*this = Color(0, 255, 0, 255);
		break;
		case eDefaultColor::cKleinBlue:
			*this = Color(0, 47, 167, 255);
		break;
		case eDefaultColor::cPink:
			*this = Color(255, 192, 203, 255);
		break;
		case eDefaultColor::cCrimson:
			*this = Color(220, 20, 60, 255);
		break;
		case eDefaultColor::cPalevioletred:
			*this = Color(219, 112, 147, 255);
		break;
		case eDefaultColor::cHotPink:
			*this = Color(255, 105, 180, 255);
		break;
		case eDefaultColor::cMediumvioletred:
			*this = Color(199, 21, 133, 255);
		break;
		case eDefaultColor::cOrchid:
			*this = Color(218, 112, 214, 255);
		break;
		default:
			*this = Color(255, 255, 255, 255);
	}
}

Color::Color(const float* ColorPointer) {
	this->r = FloatToUint(fminf(fmaxf(ColorPointer[0] * 255.f, 0.f), 255.f));
	this->g = FloatToUint(fminf(fmaxf(ColorPointer[1] * 255.f, 0.f), 255.f));
	this->b = FloatToUint(fminf(fmaxf(ColorPointer[2] * 255.f, 0.f), 255.f));
	this->a = FloatToUint(fminf(fmaxf(ColorPointer[3] * 255.f, 0.f), 255.f));
}

Color::Color(ImColor Imcolor) {
	this->r = static_cast<uint8_t>(Imcolor.Value.x * 255.f);
	this->b = static_cast<uint8_t>(Imcolor.Value.y * 255.f);
	this->g = static_cast<uint8_t>(Imcolor.Value.z * 255.f);
	this->a = static_cast<uint8_t>(Imcolor.Value.w * 255.f);
}

Color::Color(const float Alpha) {
	this->r = 0;
	this->b = 0;
	this->g = 0;
	this->a = FloatToUint(Alpha);
}

Color::Color(std::uint8_t Alpha) {
	this->r = 0;
	this->b = 0;
	this->g = 0;
	this->a = Alpha;
}

Color::Color(std::uint8_t* ColorPointer) {
	this->r = ColorPointer[0];
	this->g = ColorPointer[1];
	this->b = ColorPointer[2];
	this->a = ColorPointer[3];
}

Color::Color(std::uint8_t Default, std::uint8_t Alpha) {
	this->a = Alpha;
	this->r = Default;
	this->b = Default;
	this->g = Default;
}

Color::Color(std::uint8_t Red, std::uint8_t Green, std::uint8_t Blue, std::uint8_t Alpha) {
	this->r = Red;
	this->b = Blue;
	this->g = Green;
	this->a = Alpha;
}

Color::~Color() {
	this->r = 0;
	this->b = 0;
	this->g = 0;
	this->a = 255;
}

Color Color::Clone() const {
	return { this->r, this->g, this->b, this->a };
}

std::uint32_t Color::ToHex() const {
	std::uint32_t OutHex = 0;
	OutHex = static_cast<std::uint32_t>(this->r) << 0;
	OutHex |= static_cast<std::uint32_t>(this->g) << 8;
	OutHex |= static_cast<std::uint32_t>(this->b) << 16;
	OutHex |= static_cast<std::uint32_t>(this->a) << 24;
	return OutHex;
}

Color Color::AlphaModifier(const float Alpha) {
	this->a = FloatToUint(Alpha);
	return *this;
}

Color Color::AlphaFactor(const float Alpha) {
	this->a = FloatToUint((float)this->a * Alpha);
	return *this;
}

Color Color::AlphaModulated(const float Alpha) const {
	return { this->r, this->g, this->b, (uint8_t)(static_cast<float>(this->a) * Alpha) };
}

Color Color::AlphaModulate(const float Alpha) const {
	return { this->r, this->g, this->b, FloatToUint(Alpha) };
}

ImColor Color::ToImColor() const {
	return ImColor(
		this->r,
		this->g,
		this->b,
		this->a
	);
}