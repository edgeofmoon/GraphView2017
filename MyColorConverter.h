#pragma once

#include "MyColor4.h"
#include <cmath>

typedef MyColor4f rgba;

class MyColorConverter
{
public:
	MyColorConverter();
	~MyColorConverter();

	class Xyz{
	public:
		Xyz(){};
		Xyz(float _x, float _y, float _z){
			x = _x;
			y = _y;
			z = _z;
		}
		float x, y, z;
	};


	class Lab{
	public:
		Lab(){};
		Lab(float _l, float _a, float _b){
			l = _l;
			a = _a;
			b = _b;
		}
		float l, a, b;

		float distanceSquared(const Lab& other){
			float dl = other.l - l;
			float da = other.a - a;
			float db = other.b - b;
			return dl*dl + da*da + db*db;
		}
	};

	static Xyz rgb2xyz(int R, int G, int B)
	{
		//R from 0 to 255
		//G from 0 to 255
		//B from 0 to 255
		float r = (float)R / 255;
		float g = (float)G / 255;
		float b = (float)B / 255;

		if (r > 0.04045){ r = powf((r + 0.055) / 1.055, 2.4); }
		else { r = r / 12.92; }
		if (g > 0.04045){ g = powf((g + 0.055) / 1.055, 2.4); }
		else { g = g / 12.92; }
		if (b > 0.04045){ b = powf((b + 0.055) / 1.055, 2.4); }
		else { b = b / 12.92; }

		r = r * 100;
		g = g * 100;
		b = b * 100;

		//Observer. = 2? Illuminant = D65
		Xyz xyz;
		xyz.x = r * 0.4124 + g * 0.3576 + b * 0.1805;
		xyz.y = r * 0.2126 + g * 0.7152 + b * 0.0722;
		xyz.z = r * 0.0193 + g * 0.1192 + b * 0.9505;
		return xyz;
	}


	static Lab xyz2lab(float X, float Y, float Z)
	{

		const float REF_X = 95.047; // Observer= 2? Illuminant= D65
		const float REF_Y = 100.000;
		const float REF_Z = 108.883;

		float x = X / REF_X;
		float y = Y / REF_Y;
		float z = Z / REF_Z;

		if (x > 0.008856) { x = powf(x, 1.f / 3); }
		else { x = (7.787 * x) + (16.f / 116); }
		if (y > 0.008856) { y = powf(y, 1.f / 3); }
		else { y = (7.787 * y) + (16.f / 116); }
		if (z > 0.008856) { z = powf(z, 1.f / 3); }
		else { z = (7.787 * z) + (16.f / 116); }

		Lab lab;
		lab.l = (116 * y) - 16;
		lab.a = 500 * (x - y);
		lab.b = 200 * (y - z);

		return lab;
	}

	static Xyz lab2xyz(float l, float a, float b)
	{

		const float REF_X = 95.047; // Observer= 2? Illuminant= D65
		const float REF_Y = 100.000;
		const float REF_Z = 108.883;

		float y = (l + 16) / 116;
		float x = a / 500 + y;
		float z = y - b / 200;

		if (powf(y, 3) > 0.008856) { y = powf(y, 3); }
		else { y = (y - 16 / 116) / 7.787; }
		if (powf(x, 3) > 0.008856) { x = powf(x, 3); }
		else { x = (x - 16 / 116) / 7.787; }
		if (powf(z, 3) > 0.008856) { z = powf(z, 3); }
		else { z = (z - 16 / 116) / 7.787; }

		Xyz xyz;
		xyz.x = REF_X * x;
		xyz.y = REF_Y * y;
		xyz.z = REF_Z * z;

		return xyz;
	}

	static rgba xyz2rgb(float X, float Y, float Z)
	{
		//X from 0 to  95.047      (Observer = 2? Illuminant = D65)
		//Y from 0 to 100.000
		//Z from 0 to 108.883

		float x = X / 100;
		float y = Y / 100;
		float z = Z / 100;

		float r = x * 3.2406 + y * -1.5372 + z * -0.4986;
		float g = x * -0.9689 + y * 1.8758 + z * 0.0415;
		float b = x * 0.0557 + y * -0.2040 + z * 1.0570;

		if (r > 0.0031308) { r = 1.055 * powf(r, (1 / 2.4)) - 0.055; }
		else { r = 12.92 * r; }
		if (g > 0.0031308) { g = 1.055 * powf(g, (1 / 2.4)) - 0.055; }
		else { g = 12.92 * g; }
		if (b > 0.0031308) { b = 1.055 * powf(b, (1 / 2.4)) - 0.055; }
		else { b = 12.92 * b; }

		rgba rgb;
		rgb.r = r;
		rgb.g = g;
		rgb.b = b;

		return rgb;
	}

	static Lab rgb2lab(const rgba& rgb)
	{
		int R = rgb.r * 255;
		int G = rgb.g * 255;
		int B = rgb.b * 255;
		Xyz xyz = rgb2xyz(R, G, B);
		return xyz2lab(xyz.x, xyz.y, xyz.z);
	}

	static Lab rgb2lab(int R, int G, int B)
	{
		Xyz xyz = rgb2xyz(R, G, B);
		return xyz2lab(xyz.x, xyz.y, xyz.z);
	}

	static rgba lab2rgb(float l, float a, float b)
	{
		Xyz xyz = lab2xyz(l, a, b);
		return xyz2rgb(xyz.x, xyz.y, xyz.z);
	}
};

