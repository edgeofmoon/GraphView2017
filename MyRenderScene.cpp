#include "MyRenderScene.h"

MyRenderScene::MyRenderScene(void)
{
}


MyRenderScene::~MyRenderScene(void)
{
	for(int i = 0;i<mLights.size();i++){
		mLights[i].Destory();
	}
}

void MyRenderScene::Build(){
	MyScene::Build();
	mLights.push_back(MyLight());
	mLights[0].Generate();
	mLights[0].SetAmbient(MyVec4f(0.8f,0.8f,0.8f,1.f));
	mLights[0].SetDiffuse(MyVec4f(0.9f,0.9f,0.9f,1.f));
	mLights[0].SetSpecular(MyVec4f(0.9,0.9,0.9,1.f));
	MyLightingTool::SetLightProperty(mLights[0]);

	mDefaultMaterial.SetAmbient(MyVec4f(1.0f,1.0f,1.0f,1.f));
	mDefaultMaterial.SetDiffuse(MyVec4f(1.0,1.0,1.0,1.f));
	mDefaultMaterial.SetSpecular(MyVec4f(0.5,0.5,0.5,1.f));
	mDefaultMaterial.SetShininess(50);
	MyLightingTool::SetMaterial(mDefaultMaterial);
}

void MyRenderScene::Show(){
	MyLightingTool::EnableLighting();
	MyLightingTool::EnableLight(mLights[0]);
	MyLightingTool::EnableMaterial();
	MyScene::Show();
	MyLightingTool::DisableMaterial();
	MyLightingTool::DisableLight(mLights[0]);
	MyLightingTool::DisableLighting();
}