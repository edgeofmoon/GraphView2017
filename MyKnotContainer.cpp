#include "MyKnotContainer.h"

#include <algorithm>
using namespace std;


MyKnotContainer::MyKnotContainer(void)
{
}


MyKnotContainer::~MyKnotContainer(void)
{
	for(int i = 0;i<mKnots.size();i++){
		delete mKnots[i];
	}
	mKnots.clear();
}

int MyKnotContainer::AddKnot(MyKnot* Knot){
	if(0 == Knot) return -1;
	if(find(mKnots.begin(),mKnots.end(),Knot)==mKnots.end()){
		mKnots.push_back(Knot);
		return 1;
	}
	return 0;
}

int MyKnotContainer::RemoveKnot(MyKnot* Knot){
	if(0 == Knot) return -1;
	vector<MyKnot*>::iterator itr 
		= find(mKnots.begin(),mKnots.end(),Knot);
	if(itr!=mKnots.end()){
		mKnots.erase(itr);
		return 1;
	}
	return 0;
}

MyKnotContainer& operator << (MyKnotContainer& Knots , MyKnot* Knot){
	Knots.AddKnot(Knot);
	return Knots;
}