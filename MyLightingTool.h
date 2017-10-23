#pragma once

#include "MyVec.h"

#define MYLIGHT_MAX_NUM 8

class MyLight{
public:
	MyLight();
	~MyLight();

	bool Generate();
	void Destory();
	void SetAmbient(const MyVec4f& val);
	void SetDiffuse(const MyVec4f& val);
	void SetSpecular(const MyVec4f& val);
	void SetPosition(const MyVec4f& val);
	void SetAttenuation(const MyVec3f& val);
	void SetConstAttenuation(float val);
	void SetLinearAttenuation(float val);
	void SetQuadraticAttenuation(float val);

	int GetLightIndex() const;
	const MyVec4f& GetAmbient() const;
	const MyVec4f& GetDiffuse() const;
	const MyVec4f& GetSpecular() const;
	const MyVec4f& GetPosition() const;
	const MyVec3f& GetAttenuation() const;
	float GetConstAttenuation() const;
	float GetLinearAttenuation() const;
	float GetQuadraticAttenuation() const;

protected:
	int mLightIndex;
	MyVec4f mAmbient;
	MyVec4f mDiffuse;
	MyVec4f mSpecular;
	MyVec4f mPosition;
	MyVec3f mAttenuation;

	static MyVec<bool,MYLIGHT_MAX_NUM> LightUsed;
};

class MyMaterial{
public:
	MyMaterial();
	~MyMaterial();
	
	void SetAmbient(const MyVec4f& val);
	void SetDiffuse(const MyVec4f& val);
	void SetSpecular(const MyVec4f& val);
	void SetEmission(const MyVec4f& val);
	void SetShininess(float val);
	
	const MyVec4f& GetAmbient() const;
	const MyVec4f& GetDiffuse() const;
	const MyVec4f& GetSpecular() const;
	const MyVec4f& GetEmission() const;
	float GetShininess() const;

protected:
	MyVec4f mAmbient;
	MyVec4f mDiffuse;
	MyVec4f mSpecular;
	MyVec4f mEmission;
	float mShininess;
};

class MyLightingTool
{
public:
	MyLightingTool(void);
	~MyLightingTool(void);

	static void EnableLighting();
	static void EnableLight(const MyLight& light);
	static void DisableLighting();
	static void DisableLight(const MyLight& light);
	static void SetLightProperty(const MyLight& light);

	static void EnableMaterial();
	static void DisableMaterial();
	static void SetMaterial(const MyMaterial& material);
	
	/*
	static void SetLightAmbient(int lighti, const MyVec4f& val);
	static void SetLightDiffuse(int lighti, const MyVec4f& val);
	static void SetLightSpecular(int lighti, const MyVec4f& val);
	static void SetLightPosition(int lighti, const MyVec4f& val);
	*/
};

