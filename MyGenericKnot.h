#pragma once

#include <string>
#include "MyVec.h"
#include "MyBoundingBox.h"
#include "MyTrackBall.h"

/*
	This class is used to layout the knots.
	The layout algorithm can translate/scale the knot,
	but never rotate it.
	If rotation is needed, rotate the layout inside the knot
*/

class MyGenericNode;

class MyGenericKnot
{
public:
	MyGenericKnot();
	virtual ~MyGenericKnot();

	virtual std::string GetName() const { return mName; };
	virtual void SetName(const std::string &name){
		mName = name;
	}

	const MyBoundingBox& GetBoundingBox() const{
		return mBoundingBox;
	}
	
	MyBoundingBox GetBoundingBox(){
		return mBoundingBox;
	}
	
	void SetDrawBoundingBox(bool draw = true){
		mDrawBoundingBox = draw;
	}

	const MyTrackBall& GetTrackBall() const{
		return mTrackBall;
	}
	
	MyTrackBall& GetTrackBall(){
		return mTrackBall;
	}

	virtual void SetToDefault();
	virtual void Translate(const MyVec3f& offset);
	virtual void Scale(float sc);

	virtual void SetShowMode(int mode) {mShowMode = mode;};
	virtual void Build() = 0;
	virtual void Show() = 0;
	virtual void Destory() = 0;
	virtual bool SeeAny() = 0;
	virtual MyGenericNode* MakeSeeWhat() = 0;
	/*
	// mouse functions
	virtual int PointerFunction(int button, int state, int x, int y) = 0;
	virtual int PointerMotionFunc(int x, int y) = 0;
	virtual int PointerPassiveMotionFunc(int x, int y) = 0;

	// keyboard functions
	virtual int SemanticFunction(unsigned char key, int x, int y) = 0;
	*/

protected:
	std::string mName;
	MyBoundingBox mBoundingBox;
	MyTrackBall mTrackBall;
	bool mDrawBoundingBox;
	int mShowMode;
};

