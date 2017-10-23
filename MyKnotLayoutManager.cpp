#include "MyKnotLayoutManager.h"
#include "VisConfig.h"
#include <algorithm>
#include <cmath>

#include "MyGraphKnot.h"
#include "MyGraphMatrixEncodingRepresentation.h"
#include "MyGraphRadialLayout.h"
using namespace std;

MyKnotLayoutManager::MyKnotLayoutManager(){
	mKnotContainer = 0;
}

MyKnotLayoutManager::~MyKnotLayoutManager(void)
{
}

void MyKnotLayoutManager::Update(){
	this->initLayout();
}

void MyKnotLayoutManager::SetKnotContainer(const MyKnotContainer* knotContainer){
	mKnotContainer = knotContainer;
}

void MyKnotLayoutManager::initLayout(){
	//int r = (int)sqrtf(mKnotContainer.GetNumberOfKnot());
	/*
	for(vector<MyKnot*>::const_iterator
		itrKnot = mKnotContainer->Begin();
		itrKnot != mKnotContainer->End();
		itrKnot ++){
		int x = i%2;
		int y = i/2;
		MyVec3f center(x*size-size,y*size-size/2,-distance);
		(*itrKnot)->SetToDefault();
		(*itrKnot)->Scale(3.00f);
		(*itrKnot)->Translate(center);
		i++;
	}
	*/
	int n = mKnotContainer->GetNumberKnots();
	int i = 0;
	float size = SINGLEGRAPHSIZE;
	if(n>1) size = DOUBLEGRAPHSIZE;
	MyGraphKnot* graphknot = dynamic_cast<MyGraphKnot*>(*mKnotContainer->Begin());
	float distance = 930.f;
	float space = 1760/n;
	float offset = 0;
	float y = 0;
	if (dynamic_cast<const MyGraphRadialLayout*> (graphknot->GetLayout())
		!= 0){
			// ring
		offset = -30;
		y = 50;
	}
	else if (dynamic_cast<const MyGraphMatrixEncodingRepresentation*> (graphknot->GetRepresentation())
		!= 0){
			// matrix
		offset = -70;
		y = -20;
		space = 1910/n;
	}
	else// (dynamic_cast<const MyGraphRadialLayout*> (graphknot->GetLayout())== 0)
	{
			// projection
		offset = -30;
		y = 30;
	}
	for(vector<MyKnot*>::const_iterator
		itrKnot = mKnotContainer->Begin();
		itrKnot != mKnotContainer->End();
		itrKnot ++){
		float x = (i - (n/2.f))*space+offset;
		MyVec3f center(x+space/2,y,-distance);
		//MyVec3f center(0,0,-distance);
		(*itrKnot)->SetToDefault();
		(*itrKnot)->Scale(size);
		(*itrKnot)->Translate(center);
		i++;
	}
}

const MyKnot* MyKnotLayoutManager::GetKnot(int i) const{
	return mKnotContainer->GetKnot(i);
}
int MyKnotLayoutManager::GetNumKnots() const{
	return mKnotContainer->GetNumberKnots();
}