#pragma once

#include "MyMatrix.h"
#include "MyTrackBall.h"
#include "MyGenericEventListener.h"

class MyScene;
class MyKnot;
class MyGenericNode;

class MyView
	: public MyGenericEventListener
{
public:
	MyView(void);
	MyView(MyScene* scene);
	~MyView(void);

	virtual void Build();
	virtual void Show();
	virtual void SetViewport(const MyVec4i &viewport);
	virtual void SetScene(MyScene* scene);
	virtual MyKnot* GetKnotAt(int px, int py);
	virtual MyGenericNode* MakeSeeWhat(int px, int py, MyKnot* knot = 0);

	virtual void LoadView();
	virtual void LoadLeftEyeView();
	virtual void LoadRightEyeView();
	// peep through this pixel
	// used to pick at cursor
	virtual void LoadPeepholeView(int x, int y, int w = 3, int h = 3);
	virtual int EventHandler(MyGenericEvent& eve);

protected:
	
	virtual int keyPressEventHandler(MyGenericEvent& eve);
	virtual int mousePressEventHandler(MyGenericEvent& eve);
	virtual int mouseReleaseEventHandler(MyGenericEvent& eve);
	virtual int mouseMoveEventHandler(MyGenericEvent& eve);
	virtual int windowResizeEventHandler(MyGenericEvent& eve);
	virtual int idleEventHandler(MyGenericEvent& eve);

	MyScene* mScene;
	int mWidth;
	int mHeight;

	MyVec4i mViewport;
	MyMatrixf mProjectionMatrix;
	MyMatrixf mModelViewMatrix;
	MyTrackBall mTrackBall;
};

