#pragma once

#include "MyScale.h"
#include "MyColor4.h"
#include "MyTextureHelper.h"
#include "MyArray.h"
#include "MyString.h"
#include "MyBox.h"

namespace MyVis{
	enum MyVisualElement{
		NONE = 0,
		LENGTH = 1,
		SIZE = 2,
		ANGLE = 3,
		SATURATION = 4,
		HUE = 5,
		TEXTURE = 6,
		DENSITY = 7,
		SHAPE = 8,
		SLOPE = 9
	};

	MyString GetVisualElementName(const MyVisualElement& ele);
}

class MyVisualElementSingleValueMapper{
public:
	MyVisualElementSingleValueMapper();
	~MyVisualElementSingleValueMapper();

	virtual float GetMaxLength() const;
	virtual void SetScale(MyScale<float, float> scale);
	virtual void SetVisualElement(MyVis::MyVisualElement ele);
	virtual void DrawValueMarker(float value) const;

	virtual void DrawDefaultMarker(float value) const;
	virtual void DrawLengthMarker(float value) const;
	virtual void DrawSizeMarker(float value) const;
	virtual void DrawAngleMarker(float value) const;
	virtual void DrawSlopeMarker(float value) const;
	virtual void DrawSaturationMarker(float value) const;
	virtual void DrawHueMarker(float value) const;
	virtual void DrawTextureMarker(float value) const;
	virtual void DrawDensityMarker(float value) const;
	virtual void DrawShapeMarker(float value) const;

	virtual void DrawLengtHighlight(float value) const;
	virtual void DrawSizeHighlight(float value) const;
	virtual void DrawAngleHighlight(float value) const;
	virtual void DrawSlopeHighlight(float value) const;
	virtual void DrawSaturationHighlight(float value) const;
	virtual void DrawHueHighlight(float value) const;
	virtual void DrawTextureHighlight(float value) const;
	virtual void DrawDensityHighlight(float value) const;
	virtual void DrawShapeHighlight(float value) const;

	virtual const MyScale<float, float>& GetScale() const;
	virtual const MyScale<float, MyColor4f>& GetSaturationScale() const;
	virtual const MyScale<float, MyColor4f>& GetHueScale() const;

	virtual void SetRandomSeed(int i);
	virtual void SetHighlightMode(bool b = true);
	virtual void SetHighlightColor(const MyColor4f& color);

	virtual void DrawHighlightBack(float value) const;

	virtual MyBox2f GetMarkerBox(float value) const;

	// the random number generated might be differet
	// for each run. But within one run, it is determined
	// only by i and mRandomSeed;
	int GetRand(int i) const;

	static int mBackground;

protected:
	MyScale<float, float> mScale;
	MyScale<float, MyColor4f> mSaturationScale;
	MyScale<float, MyColor4f> mHueScale;
	MyVis::MyVisualElement mVisualElement;
	float mHighlightBoarder;
	
	MyTextureHelper mTextureHelper;
	MyHighlightTextureHelper mHighlightTextureHelper;

	int mRandomSeed;
	MyArrayi mRands;

	MyArray2f mCircularSampleArray;

	bool mHighlightMode;
	MyColor4f mHighlightColor;
};