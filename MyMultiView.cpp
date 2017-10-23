#include "MyMultiView.h"
#include "MyGraphicsTool.h"
#include "MyScene.h"
#include "MyGenericHierarchyOverlay.h"


MyMultiView::MyMultiView(void)
{
	mViewport = MyGraphicsTool::GetViewport();
	mTrackBall.Reshape(mViewport[2],mViewport[3]);
}


MyMultiView::~MyMultiView(void)
{
}

void MyMultiView::Show(){
	//MyGraphicsTool::PushProjectionMatrix();
	//MyGraphicsTool::PushMatrix();
	this->LoadView();
	for(int i = 0;i<mScene->GetNumKnots();i++){
		
		MyGraphicsTool::PushMatrix();
		this->enableClipPlanes(i);
		MyGraphicsTool::LoadTrackBall(&mTrackBall);
		mScene->GetKnot(i)->Show();
		this->disableClipPlanes(i);
		

		MyGraphicsTool::PopMatrix();

		this->DrawClipPlanes(i);

	}
	//MyGraphicsTool::PopMatrix();
	//MyGraphicsTool::PopProjectionMatrix();
}

void MyMultiView::SetScene(MyScene* scene){
	mScene = scene;
}

MyKnot* MyMultiView::GetKnotAt(int px, int py){
	/*
	MyKnot* rst = 0;
	MyGraphicsTool::PushProjectionMatrix();
	MyGraphicsTool::PushMatrix();
	this->LoadPeepholeView(px, py);
	bool found = false;
	for(int i = mScene->GetNumKnots()-1;i>=0 && !found;i--){
		MyGraphicsTool::PushMatrix();
		this->enableClipPlanes(i);
		MyGraphicsTool::LoadTrackBall(&mTrackBall);
		if(mScene->GetKnot(i)->SeeAny()){
			rst = mScene->GetKnot(i);
			found = true;
		}
		this->disableClipPlanes(i);
		MyGraphicsTool::PopMatrix();

	}
	MyGraphicsTool::PopMatrix();
	MyGraphicsTool::PopProjectionMatrix();
	return rst;
	*/
	int idx = this->getViewportIndex(px, py);
	MyKnot*  rst = 0;
	if(idx >= 0){
		MyGraphicsTool::PushProjectionMatrix();
		MyGraphicsTool::PushMatrix();
		this->LoadPeepholeView(px, py);
		MyGraphicsTool::LoadTrackBall(&mTrackBall);
		MyKnot* knot = mScene->GetKnot(idx);
		if (knot->SeeAny()){
			rst = knot;
		}
		MyGraphicsTool::PopMatrix();
		MyGraphicsTool::PopProjectionMatrix();
	}
	return rst;
	/*
	this->LoadPeepholeView(px, py);
	for (int i = 0; i < mScene->GetNumKnots(); i++){
		MyGraphicsTool::PushMatrix();
		//this->enableClipPlanes(i);
		MyGraphicsTool::LoadTrackBall(&mTrackBall);
		MyKnot* knot = mScene->GetKnot(i);
		if (knot->SeeAny()){
			//this->disableClipPlanes(i);
			MyGraphicsTool::PopMatrix();
			return knot;
		}
		//this->disableClipPlanes(i);
		MyGraphicsTool::PopMatrix();
	}
	return 0;
	*/
}
/*
MyGenericNode* MyMultiView::MakeSeeWhat(int px, int py, MyKnot* knot){
	MyGenericNode* rst = 0;
	this->LoadPeepholeView(px, py);
	if(knot){
		MyGraphicsTool::PushMatrix();
		MyGraphicsTool::LoadTrackBall(&mTrackBall);
		if (knot->SeeAny()){
			rst = knot->MakeSeeWhat();
		}
		MyGraphicsTool::PopMatrix();
	}
	else{
		bool saw = false;
		for (int i = 0; i < mScene->GetNumKnots()&&! saw; i++){
			MyKnot* tknot = mScene->GetKnot(i);
			this->enableClipPlanes(i);
			MyGraphicsTool::PushMatrix();
			MyGraphicsTool::LoadTrackBall(&mTrackBall);
			if (tknot->SeeAny()){
				rst = tknot->MakeSeeWhat();
				saw = true;
			}
			MyGraphicsTool::PopMatrix();
			this->disableClipPlanes(i);
		}
	}
	return rst;
}
*/
void MyMultiView::Build(){
	//mWidth = 1024;
	//mHeight = 768;
	//mWidth = mViewport[2];
	//mHeight = mViewport[3];
	//mProjectionMatrix = MyMatrixf::PerspectiveMatrix(60,mWidth/(float)mHeight,1,1000);
	//mProjectionMatrix = MyMatrixf::OrthographicMatrix(
	//	-mWidth/2,mWidth/2,-mHeight/2,mHeight/2,0.001f,1000.f);
	//mModelViewMatrix = MyMatrixf::IdentityMatrix();
	//MyGraphicsTool::SetBackgroundColor(MyColor4f(0.5f,0.5f,0.5f,0.f));

	MyView::Build();
	this->updateClipPlanes();
}
void MyMultiView::LoadView(){
	MyGraphicsTool::LoadProjectionMatrix(&mProjectionMatrix);
	MyGraphicsTool::LoadModelViewMatrix(&mModelViewMatrix);
}
// peep through this pixel
// used to pick at cursor
void MyMultiView::LoadPeepholeView(int x, int y, int w, int h){
	MyGraphicsTool::LoadPickMatrix(x, y, w, h, &mProjectionMatrix,mViewport);
	MyGraphicsTool::LoadModelViewMatrix(&mModelViewMatrix);
}

int MyMultiView::mouseMoveEventHandler(MyGenericEvent& eve){
	 return 0;
	MyGenericEvent::MyMouseKey _mouseKey = eve.GetEventMouseKey();
	int _x = eve.GetMouseX();
	int _y = eve.GetMouseY();
	float scaleIdx;
	switch (_mouseKey){
	case MyGenericEvent::MOUSE_KEY_LEFT:
		//mTrackBall.RotateMotion(x, y);
		break;
	case MyGenericEvent::MOUSE_KEY_MIDDLE:
		//mTrackBall.TranslateMotionX(_x, _y);
		//eve.SetNeedRedraw();
		break;
	case MyGenericEvent::MOUSE_KEY_RIGHT:
		//mTrackBall.ZoomMotion(x, y);
		// scale all
		//scaleIdx = mTrackBall.GetScaleFromMotion(_x, _y);
		//for(int i = 0;i<mScene->GetNumKnots();i++){
		//	mScene->GetKnot(i)->Scale(scaleIdx);
		//}
		//eve.SetNeedRedraw();
		break;
	default:
		break;

	}
	return 1;
}

int MyMultiView::windowResizeEventHandler(MyGenericEvent& eve){
	mWidth = eve.GetWindowWidth();
	mHeight = eve.GetWindowHeight();
	mViewport = MyVec4i(0,0,mWidth,mHeight);
	MyGraphicsTool::SetSize(mWidth,mHeight);
	//mProjectionMatrix = MyMatrixf::PerspectiveMatrix(60,mWidth/(float)mHeight,1,1000);
	
	mProjectionMatrix = MyMatrixf::OrthographicMatrix(
		-mWidth/2,mWidth/2,-mHeight/2,mHeight/2,1.f,10000.f);
	mTrackBall.Reshape(mWidth,mHeight);
	
	this->updateClipPlanes();

	int sceneHandleResult = mScene->EventHandler(eve);
	return sceneHandleResult;
}

void MyMultiView::updateClipPlanes(){
	mViewportY = 100;
	mViewportX = 0;
	mViewportWidth = 920;
	mViewportHeight = 1138;
	mViewportBoarderX = 10;
	mViewportBoarderY = 20;
	int numKnots = mScene->GetNumKnots();
	if(numKnots == 1){
		mViewportX = 0;
		mViewportWidth = 1760;
	}
	mClipPlanes.clear();
	for(int i = 0;i<mScene->GetNumKnots();i++){
		float left = mViewportX+mViewportBoarderX+(mViewportWidth+mViewportBoarderX)*i-mWidth/2;
		float right = mViewportX+mViewportBoarderX+(mViewportWidth+mViewportBoarderX)*i-mWidth/2+mViewportWidth;
		float bottom = -mHeight/2+mViewportY;
		float top = -mHeight/2+mViewportY+mViewportHeight;
		MyArray4f clipPlane;
		//MyVec4f equation_bottom(0.f,1.f,0.f,-bottom);
		//MyVec4f equation_top(0,-1,0,top);
		//MyVec4f equation_left(1,0,0,-left);
		//MyVec4f equation_right(-1,0,0,right);
		clipPlane << MyVec4f(0.f,1.f,0.f,-bottom);
		clipPlane << MyVec4f(0,-1,0,top);
		clipPlane << MyVec4f(1,0,0,-left);
		clipPlane << MyVec4f(-1,0,0,right);
		mClipPlanes << clipPlane;
	}
}

void MyMultiView::enableClipPlanes(int idx){
	return;
	for(int j = 0;j<mClipPlanes[idx].size();j++){
		MyGraphicsTool::EnableClipPlane(j);
		MyGraphicsTool::SetClipPlane(j, mClipPlanes[idx][j]);
	}
}

void MyMultiView::disableClipPlanes(int idx){
	return;
	for(int j = 0;j<mClipPlanes[idx].size();j++){
		MyGraphicsTool::DisableClipPlane(j);
	}
}

void MyMultiView::DrawClipPlanes(int idx){
	return;
	float distance = 730;
	MyGraphicsTool::Color(MyColor4f(1,1,1,1));
	float bottom = -mClipPlanes[idx][0][3];
	float top = mClipPlanes[idx][1][3];
	float left = -mClipPlanes[idx][2][3];
	float right = mClipPlanes[idx][3][3];

	MyGraphicsTool::BeginLineLoop();
	MyGraphicsTool::Vertex(MyVec3f(left,bottom,-distance));
	MyGraphicsTool::Vertex(MyVec3f(right,bottom,-distance));
	MyGraphicsTool::Vertex(MyVec3f(right,top,-distance));
	MyGraphicsTool::Vertex(MyVec3f(left,top,-distance));
	MyGraphicsTool::EndPrimitive();
}

int MyMultiView::getViewportIndex(int x, int y) const{
	y = mHeight - y;
	x -= mWidth/2;
	y -= mHeight/2;
	for(int idx = 0;idx<mClipPlanes.size();idx++){
		float bottom = -mClipPlanes[idx][0][3];
		float top = mClipPlanes[idx][1][3];
		float left = -mClipPlanes[idx][2][3];
		float right = mClipPlanes[idx][3][3];
		if(x >= left && x <= right
			&& y >= bottom && y <= top){
			return idx;
		}
	}
	return -1;
}