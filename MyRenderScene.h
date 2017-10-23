#pragma once
#include "myscene.h"
#include "MyLightingTool.h"

#include <vector>

class MyRenderScene :
	public MyScene
{
public:
	MyRenderScene(void);
	~MyRenderScene(void);
	
	virtual void Build();
	virtual void Show();

protected:
	std::vector<MyLight> mLights;
	MyMaterial mDefaultMaterial;
};

