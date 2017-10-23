#pragma once

#include "MyKnotLayoutManager.h"
#include "MyGenericEventListener.h"
#include "MyKnotContainer.h"

class MyView;

class MyScene
	: public MyGenericEventListener
{
public:
	MyScene(void);
	~MyScene(void);

	virtual void SetView(MyView* view);

	virtual void Show();
	virtual void Build();
	
	virtual int AddKnot(MyKnot* pKnot);
	virtual int RemoveKnot(MyKnot* pKnot);

	virtual int EventHandler(MyGenericEvent& eve);

	virtual MyKnot* GetKnot(int i);
	virtual const MyKnot* GetKnot(int i) const;
	virtual int GetNumKnots() const;

	virtual void SetInteractionMode(int mode){mInteractionMode = mode;}
protected:
	MyView* mView;
	MyKnot* mFocusKnot;
	//virtual int keyReleaseEventHandler(unsigned char, int, int){};
	//virtual int keyPressEventHandler(unsigned char, int, int){};
	//virtual int mousePressEventHandler(int, int , int){};
	//virtual int mouseReleaeEventHandler(int, int , int){};
	//virtual int mouseMoveEventHandler(int, int){};
	MyKnotLayoutManager mLayoutManager;

	MyKnotContainer mKnotContainer;

	void hintFocusKnot();
	int mInteractionMode;
};

