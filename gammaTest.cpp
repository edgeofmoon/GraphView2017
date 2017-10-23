#include "MyScale.h"
#include "MyColor4.h"
#include "MyGraphicsTool.h"
#include "MyPrimitiveDrawer.h"
#include "MyString.h"

MyGraphicsTool helper;
int my;
float gamma = 2.68;
int numSample = 200;
MyScale<float, MyColor4f> colorScale;

void DisplayFunc(void){
	helper.ClearFrameBuffer();
	
	MyGraphicsTool::BeginQuadStrip();
	for(int i = 0;i<=numSample;i++){
		float value = i/(float)numSample;
		float x = i/(float)numSample*2-1;
		MyColor4f color = colorScale[value];
		MyGraphicsTool::Color(color);
		MyGraphicsTool::Vertex(MyVec3f(x,-1,-2));
		MyGraphicsTool::Vertex(MyVec3f(x,-0.0,-2));
	}
	MyGraphicsTool::EndPrimitive();
	MyPrimitiveDrawer::Color(MyColor4f::white());
	MyPrimitiveDrawer::DrawBitMapTextLarge(MyVec3f(0,0.8,-2),MyString(gamma));
	helper.FreshScreen();

}
void ReshapeFunc(int w, int h){
	helper.SetSize(w,h);
	MyMatrixf mModelViewMatrix = MyMatrixf::IdentityMatrix();
	MyGraphicsTool::LoadModelViewMatrix(&mModelViewMatrix);
	MyVec4i viewport = MyGraphicsTool::GetViewport();
	int mWidth = viewport[2];
	int mHeight = viewport[3];
	//mProjectionMatrix = MyMatrixf::PerspectiveMatrix(60,mWidth/(float)mHeight,1,1000);
	MyMatrixf mProjectionMatrix = MyMatrixf::OrthographicMatrix(
		-1,1,-1,1,1.f,1000.f);
	MyGraphicsTool::LoadProjectionMatrix(&mProjectionMatrix);
}
void KeyReleaseFunc(unsigned char c, int x, int y){
}
void KeyPressFunc(unsigned char c, int x, int y){
}
void MouseKeyFunc(int button, int state, int x, int y){
	my = y;
}
void MouseMoveFunc(int x, int y){
	gamma += (my-y)/100.f;
	my = y;
	colorScale.GetRange().SetGamma(gamma);
	helper.Update();
}

void IdleFunc(){
}


int main(int argc, char* argv[]){
	
	MyArray<MyColor4f> hues;
	hues << MyColor4f(0.847,0.057,0.057)
		<< MyColor4f(0.527,0.527,0.000)
		<< MyColor4f(0.000,0.592,0.000)
		<< MyColor4f(0.000,0.559,0.559)
		<< MyColor4f(0.316,0.316,0.991)
		<< MyColor4f(0.718,0.000,0.718);
		//<< MyColor4f(0.847,0.057,0.057);
	colorScale.SetDomain(0,1);
	colorScale.SetRange(hues);
	colorScale.GetRange().SetInterpolationMethod(My1DSampler<MyColor4f>::INTERPOLATE_GAMMA);
	colorScale.GetRange().SetGamma(gamma);

	helper.Init(&argc,argv);

	helper.RegisterDisplayFunction(DisplayFunc);
	helper.RegisterReshapeFunction(ReshapeFunc);
	helper.RegisterKeyReleaseFunction(KeyReleaseFunc);
	helper.RegisterKeyPressFunction(KeyPressFunc);
	helper.RegisterMouseKeyFunction(MouseKeyFunc);
	helper.RegisterMouseMoveFunction(MouseMoveFunc);
	helper.RegisterIdleFunction(IdleFunc);
	helper.Start();

	return 1;
}