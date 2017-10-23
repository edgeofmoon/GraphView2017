#include "MyLightingTool.h"

#include "gl/glut.h"

MyVec<bool,8> MyLight::LightUsed = MyVec<bool,8>();

MyLight::MyLight(){
	mLightIndex = -1;
	mAmbient = MyVec4f(0.f,0.f,0.f,1.f);
	mDiffuse = MyVec4f(1.f,1.f,1.f,1.f);
	mSpecular = MyVec4f(1.f,1.f,1.f,1.f);
	mPosition = MyVec4f(0.f,0.f,1.f,0.f);
	mAttenuation = MyVec3f(1.f,0.f,0.f);
}
MyLight::~MyLight(){
}

bool MyLight::Generate(){
	for(int i = 0;i<MYLIGHT_MAX_NUM;i++){
		if(!LightUsed[i]){
			mLightIndex = i;
			LightUsed[i] = true;
			return true;
		}
	}
	return false;
}

void MyLight::Destory(){
	if(mLightIndex>=0 && mLightIndex<MYLIGHT_MAX_NUM){
		LightUsed[mLightIndex] = false;
	}
}
void MyLight::SetAmbient(const MyVec4f& val){
	mAmbient = val;
}
void MyLight::SetDiffuse(const MyVec4f& val){
	mDiffuse = val;
}
void MyLight::SetSpecular(const MyVec4f& val){
	mSpecular = val;
}
void MyLight::SetPosition(const MyVec4f& val){
	mPosition = val;
}
void MyLight::SetAttenuation(const MyVec3f& val){
	mAttenuation = val;
}
void MyLight::SetConstAttenuation(float val){
	mAttenuation[0] = val;
}
void MyLight::SetLinearAttenuation(float val){
	mAttenuation[1] = val;
}
void MyLight::SetQuadraticAttenuation(float val){
	mAttenuation[2] = val;
}

int MyLight::GetLightIndex() const{
	return mLightIndex;
}

const MyVec4f& MyLight::GetAmbient() const{
	return mAmbient;
}
const MyVec4f& MyLight::GetDiffuse() const{
	return mDiffuse;
}
const MyVec4f& MyLight::GetSpecular() const{
	return mSpecular;
}
const MyVec4f& MyLight::GetPosition() const{
	return mPosition;
}
const MyVec3f& MyLight::GetAttenuation() const{
	return mAttenuation;
}
float MyLight::GetConstAttenuation() const{
	return mAttenuation[0];
}
float MyLight::GetLinearAttenuation() const{
	return mAttenuation[1];
}
float MyLight::GetQuadraticAttenuation() const{
	return mAttenuation[2];
}

MyMaterial::MyMaterial(){
	mAmbient = MyVec4f(0.2f, 0.2f, 0.2f, 1.0f);
	mDiffuse = MyVec4f(0.8f, 0.8f, 0.8f, 1.0f);
	mSpecular = MyVec4f(0.f, 0.f, 0.f, 1.f);
	mEmission = MyVec4f(0.f, 0.f, 0.f, 1.f);
	mShininess = 0.f;
}

MyMaterial::~MyMaterial(){
}

void MyMaterial::SetAmbient(const MyVec4f& val){
	mAmbient = val;
}
void MyMaterial::SetDiffuse(const MyVec4f& val){
	mDiffuse = val;
}
void MyMaterial::SetSpecular(const MyVec4f& val){
	mSpecular = val;
}
void MyMaterial::SetEmission(const MyVec4f& val){
	mEmission = val;
}
void MyMaterial::SetShininess(float val){
	mShininess = val;
}

const MyVec4f& MyMaterial::GetAmbient() const{
	return mAmbient;
}
const MyVec4f& MyMaterial::GetDiffuse() const{
	return mDiffuse;
}
const MyVec4f& MyMaterial::GetSpecular() const{
	return mSpecular;
}

const MyVec4f& MyMaterial::GetEmission() const{
	return mEmission;
}

float MyMaterial::GetShininess() const{
	return mShininess;
}


MyLightingTool::MyLightingTool(void)
{
}


MyLightingTool::~MyLightingTool(void)
{
}

void MyLightingTool::EnableLighting(){
	glEnable(GL_LIGHTING);
}

void MyLightingTool::EnableLight(const MyLight& light){
	glEnable(GL_LIGHT0+light.GetLightIndex());
}

void MyLightingTool::DisableLighting(){
	glDisable(GL_LIGHTING);
}

void MyLightingTool::DisableLight(const MyLight& light){
	glDisable(GL_LIGHT0+light.GetLightIndex());
}

void MyLightingTool::SetLightProperty(const MyLight& light){
	glLightf( GL_LIGHT0+light.GetLightIndex(), GL_CONSTANT_ATTENUATION, light.GetConstAttenuation() );
    glLightf( GL_LIGHT0+light.GetLightIndex(), GL_LINEAR_ATTENUATION, light.GetLinearAttenuation() );
    glLightf( GL_LIGHT0+light.GetLightIndex(), GL_QUADRATIC_ATTENUATION, light.GetQuadraticAttenuation() );
    glLightfv( GL_LIGHT0+light.GetLightIndex(), GL_POSITION, light.GetPosition().d() );
    glLightfv( GL_LIGHT0+light.GetLightIndex(), GL_AMBIENT, light.GetAmbient().d() );
    glLightfv( GL_LIGHT0+light.GetLightIndex(), GL_DIFFUSE, light.GetDiffuse().d() );
    glLightfv( GL_LIGHT0+light.GetLightIndex(), GL_SPECULAR, light.GetSpecular().d() );
}


void MyLightingTool::EnableMaterial(){
	glEnable ( GL_COLOR_MATERIAL ) ;
}
void MyLightingTool::DisableMaterial(){
	glDisable ( GL_COLOR_MATERIAL ) ;
}

void MyLightingTool::SetMaterial(const MyMaterial& material){
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, material.GetAmbient().d());
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material.GetDiffuse().d());
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material.GetSpecular().d());
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, material.GetEmission().d());
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, material.GetShininess());
}
/*
void MyLightingTool::SetLightAmbient(int lighti, const MyVec4f& val){
	glLightfv( GL_LIGHT0+lighti, GL_AMBIENT, val.d() );
}
void MyLightingTool::SetLightDiffuse(int lighti, const MyVec4f& val){
	glLightfv( GL_LIGHT0+lighti, GL_DIFFUSE, val.d() );
}
void MyLightingTool::SetLightSpecular(int lighti, const MyVec4f& val){
	glLightfv( GL_LIGHT0+lighti, GL_SPECULAR, val.d() );
}
void MyLightingTool::SetLightPosition(int lighti, const MyVec4f& val){
	glLightfv( GL_LIGHT0+lighti, GL_POSITION, val.d() );
}
*/
