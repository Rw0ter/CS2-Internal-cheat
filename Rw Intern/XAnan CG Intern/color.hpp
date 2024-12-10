#pragma once
#include "imgui.h"
#include <cstdint>
enum eDefaultColor : int {
	cWhite,
	cBlack,
	cBlue,
	cGreen,
	cKleinBlue,
	cPink,
	cCrimson,
	cPalevioletred,
	cHotPink,
	cMediumvioletred,
	cOrchid
};

class Vector4D;
class Color {
public:
	std::uint8_t r, g, b, a;
	Color();
	Color(ImColor Imcolor);
	Color(const float Alpha);
	Color(eDefaultColor Default);
	Color(const float* ColorPointer);
	Color(std::uint8_t Alpha);
	Color(std::uint8_t* ColorPointer);
	Color(std::uint8_t Default, std::uint8_t Alpha);
	Color(std::uint8_t Red, std::uint8_t Green, std::uint8_t Blue, std::uint8_t Alpha = 255);
	~Color();
	operator ImU32() const { return this->ToImColor(); }
	operator ImColor() const { return this->ToImColor(); }
	operator float* () { return reinterpret_cast<float*>(this); }
	std::uint8_t operator[](int index) { return reinterpret_cast<uint8_t*>(this)[index]; }
	std::uint8_t at(int index) { return index == 3 ? a : index == 2 ? b : index == 1 ? g : r; }
	static Color Black() { return Color(eDefaultColor::cBlack); }
	static Color White() { return Color(eDefaultColor::cWhite); }
	static Color Blue() { return Color(eDefaultColor::cBlue); }
	static Color Green() { return Color(eDefaultColor::cGreen); }
	static Color KleinBlue() { return Color(eDefaultColor::cKleinBlue); }
	static Color Pink() { return Color(eDefaultColor::cPink); }
	static Color Crimson() { return Color(eDefaultColor::cCrimson); }
	static Color Palevioletred() { return Color(eDefaultColor::cPalevioletred); }
	static Color HotPink() { return Color(eDefaultColor::cHotPink); }
	static Color Mediumvioletred() { return Color(eDefaultColor::cMediumvioletred); }
	static Color Orchid() { return Color(eDefaultColor::cOrchid); }
	Color Clone() const;
	uint32_t ToHex() const;
	float* ToFloat() const;
	ImColor ToImColor() const;
	void Override(const Color& Other);
	Color AlphaModulate(const float Alpha) const;
	Color AlphaModifier(const float Alpha);
	Color AlphaModulated(const float Alpha) const;
	Color AlphaFactor(const float Alpha);
};