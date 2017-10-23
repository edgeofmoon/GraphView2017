#include "MyGraphIterativeLayout.h"


MyGraphIterativeLayout::MyGraphIterativeLayout(void)
{
	mIterationLimit = -1;
	mIteration = 0;
}


MyGraphIterativeLayout::~MyGraphIterativeLayout(void)
{
}

void MyGraphIterativeLayout::LimitIterations(int n){
	mIterationLimit = n;
}

bool MyGraphIterativeLayout::Advance(){
	mIteration ++;
	return this->ShouldContinue();
}

void MyGraphIterativeLayout::AdvanceToFinal(){
	while(this->Advance());
}

bool MyGraphIterativeLayout::ShouldContinue() const{
	if(mIterationLimit>=0){
		if(mIteration<=mIterationLimit){
			return true;
		}
		return false;
	}
	return true;
}