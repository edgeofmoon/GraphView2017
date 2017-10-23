#pragma once

#include <vector>
#include "MyKnot.h"

class MyKnotContainer
{
public:
	MyKnotContainer(void);
	~MyKnotContainer(void);

	int AddKnot(MyKnot* Knot);
	int RemoveKnot(MyKnot* Knot);

	int GetNumberKnots() const{
		return mKnots.size();
	}

	std::vector<MyKnot*>::iterator Begin(){
		return mKnots.begin();
	};
	std::vector<MyKnot*>::const_iterator Begin() const{
		return mKnots.begin();
	};
	std::vector<MyKnot*>::iterator End(){
		return mKnots.end();
	};
	std::vector<MyKnot*>::const_iterator End() const{
		return mKnots.end();
	};

	MyKnot* GetKnot(int i){
		return mKnots[i];
	}

	const MyKnot* GetKnot(int i) const{
		return mKnots[i];
	}

	friend MyKnotContainer& 
		operator << (MyKnotContainer&, MyKnot*);

protected:
	std::vector<MyKnot*> mKnots;
};

