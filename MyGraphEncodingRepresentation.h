#pragma once

#include "MyGraphRepresentation.h"
#include "MyVisualElement.h"

class MyGraphEncodingRepresentation
	:public MyGraphRepresentation
{
public:
	MyGraphEncodingRepresentation(void);
	~MyGraphEncodingRepresentation(void);

	
	virtual void Show();
	virtual void Update();
	virtual void ShowNode(int i);
	virtual void ShowHighlightedNode(int i);
	virtual void ShowNodeHighlight(int i);
	virtual void SetNodeValuef(MyArrayf values);
	virtual void SetNodeEncodingMethods(MyVis::MyVisualElement ele);
	
	virtual void SetValueRange(float min, float max);
	virtual const MyScale<float, MyColor4f>& GetColorScale() const;
	virtual const MyVisualElementSingleValueMapper& GetMapper() const;
	virtual const MyScale<float, float>& GetMappingScale() const;
	virtual MyVis::MyVisualElement GetVisualElement() const;

	virtual void DrawMarker(float value, int randomSeed = 0);

	virtual void UpdataLabel();
	virtual void DrawLegend();
	virtual void ShowLabel(int idx);
	virtual void ShowLabels();
	
	virtual int GetNumOfTangibleMarker(int i) const;
	virtual MyVec3f GetNodeTangiblePos(int i, int mIdx = 0) const;
	virtual MyBoundingBox GetNodeTangibleBox(int i, int mIdx = 0) const;

protected:
	virtual void drawEncodedNode(int i);
	MyArrayf mNodeValues;
	MyScale<float, float> mMappingScale;
	MyVisualElementSingleValueMapper mMapper;
	MyVis::MyVisualElement mVisualElement;

	bool mValueRangeSet;
};

