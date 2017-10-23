#pragma once
#include "myview.h"
#include "MyArray.h"

class MyMultiView :
	public MyView
{
public:
	MyMultiView(void);
	~MyMultiView(void);

	virtual void Build();
	virtual void Show();
	//virtual void SetViewport(const MyVec4i &viewport);
	virtual void SetScene(MyScene* scene);
	virtual MyKnot* GetKnotAt(int px, int py);
	//virtual MyGenericNode* MakeSeeWhat(int px, int py, MyKnot* knot = 0);

	virtual void LoadView();
	// peep through this pixel
	// used to pick at cursor
	virtual void LoadPeepholeView(int x, int y, int w = 3, int h = 3);
	
	void DrawClipPlanes(int idx);

protected:
	virtual int mouseMoveEventHandler(MyGenericEvent& eve);
	virtual int windowResizeEventHandler(MyGenericEvent& eve);

	void updateClipPlanes();
	void enableClipPlanes(int idx);
	void disableClipPlanes(int idx);
	int getViewportIndex(int x, int y) const;
	MyArray<MyArray4f> mClipPlanes;

	int mViewportX;
	int mViewportY;
	int mViewportWidth;
	int mViewportHeight;
	int mViewportBoarderX;
	int mViewportBoarderY;
#define VIEWPORTWIDTH 880
#define VIEWPORTHIGHT 1038
#define VIEWPORTBOARDER_X 20
#define VIEWPORTBOARDER_Y 20
};

