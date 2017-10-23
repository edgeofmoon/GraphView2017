#include "MyGenericKnot.h"

MyGenericKnot::MyGenericKnot(){
	mBoundingBox = MyBoundingBox(MyVec3f(0.f,0.f,0.f),100.f,100.f,100.f);
	mShowMode = 0;
};

MyGenericKnot::~MyGenericKnot(){
}

void MyGenericKnot::SetToDefault(){
	mTrackBall.ResetTranslate();
	mTrackBall.ResetScale();
}
void MyGenericKnot::Translate(const MyVec3f& offset){
	//mBoundingBox.Translate(offset);
	mTrackBall.Translate(offset);
}
void MyGenericKnot::Scale(float sc){
	//mBoundingBox.Scale(sc);
	mTrackBall.ScaleAdd(sc-1);
}
 