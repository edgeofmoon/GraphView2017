#include "MyGraphFruchtermanReingoldLayout.h"
#include <cmath>
#include <algorithm>
MyGraphFruchtermanReingoldLayout::MyGraphFruchtermanReingoldLayout(void)
{
}


MyGraphFruchtermanReingoldLayout::~MyGraphFruchtermanReingoldLayout(void)
{
}

void MyGraphFruchtermanReingoldLayout::Update(){
	switch(mUse){
	case NO_USE:
	case USE_AS_INIT:
	case USE_AS_FINAL:
	default:
		mPos = *mInPos;
		break;
	}
	if(this->mFlat){
		this->flatThings();
	}
	this->initLayout();
}


bool MyGraphFruchtermanReingoldLayout::Advance(){
	if(this->ShouldContinue()){
		// calculate repulsive forces
		for(int i = 0;i<mGraph->GetNumNodes();i++){
			mDisplacements[i] = MyVec3f::zero();
			for(int j = 0;j<mGraph->GetNumNodes();j++){
				if(j != i){
					MyVec3f deltaPos = mPos[i]-mPos[j];
					float distance = deltaPos.norm();
					mDisplacements[i] += deltaPos/distance*repulsiveForce(distance,mForceParameter);
				}
			}
		}

		// calculate attractive forces
		for(int i = 0;i<mGraph->GetNumEdges();i++){
			MyVec2i nodeIdx = mGraph->GetEdge(i);
			if(nodeIdx[0] != nodeIdx[1]){
				MyVec3f deltaPos = mPos[nodeIdx[0]]-mPos[nodeIdx[1]];
				float distance = deltaPos.norm();
				mDisplacements[nodeIdx[0]] -= deltaPos/distance*attractiveForce(distance,mForceParameter);
				mDisplacements[nodeIdx[1]] += deltaPos/distance*attractiveForce(distance,mForceParameter);

				// add edge weight as attraction
				float weight = mGraph->GetEdgeWeight(i);
				mDisplacements[nodeIdx[0]] -= deltaPos/distance*std::abs(weight)*mForceParameter;
				mDisplacements[nodeIdx[1]] += deltaPos/distance*std::abs(weight)*mForceParameter;
			}
		}

		// limit to bounding box
		for(int i = 0;i<mGraph->GetNumNodes();i++){
			float displaceLength = mDisplacements[i].norm();
			mPos[i] += mDisplacements[i]/displaceLength*std::min(displaceLength,mTemperature);
			mPos[i] = mBoundingBox.BoundPoint(mPos[i]);
			if(mFlat){
				mPos[i][2] = 0.f;
			}
		}

		// reduce the  temperature
		cool();
		mIteration ++;
		return true;
	}
	return false;
}



bool MyGraphFruchtermanReingoldLayout::ShouldContinue() const{
	return mTemperature >= mTemperatureLimit && MyGraphIterativeLayout::ShouldContinue();
}


void  MyGraphFruchtermanReingoldLayout::flatThings(){
	for(int i = 0;i<mPos.size();i++){
		mPos[i][2] = 0.f;
	}
	// can not be exactly the same
	for(int i = 0;i<mPos.size();i++){
		for(int j = i+1;j<mGraph->GetNumNodes();j++){
			MyVec3f deltaPos = mPos[i]-mPos[j];
			if(deltaPos == MyVec3f::zero()){
				mPos[i][0]+=0.05;
			}
		}
	}
}

void MyGraphFruchtermanReingoldLayout::initLayout(){
	if(mFlat){
		float area = mBoundingBox.GetFrontFaceArea();
		mForceParameter = sqrtf(area/mGraph->GetNumNodes());
	}
	else{
		float volume = mBoundingBox.GetVolume();
		mForceParameter = powf(volume/mGraph->GetNumNodes(),1.0f/3);
	}


	mDisplacements = std::vector<MyVec3f>(mGraph->GetNumNodes(),MyVec3f::zero());

	mTemperature = 55.f;
	mIteration = 0;

	mTemperatureLimit = 0.f;
	this->LimitIterations(100);
}
void MyGraphFruchtermanReingoldLayout::cool(){
	if(mTemperature*0.95f>=0.1f){
		mTemperature *= 0.95f;
	}
	else{
		mTemperature -= 0.1f;
	}
}
const float MyGraphFruchtermanReingoldLayout::repulsiveForce(float distance, float k) const{
	return k*k/distance;
}
const float MyGraphFruchtermanReingoldLayout::attractiveForce(float distance, float k) const{
	return distance*distance/k;
}