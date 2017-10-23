#include "MyGraphLocalOptimizationLayout.h"
#include <vector>
#include <algorithm>

#define MAXDIST 99999999.f

MyGraphLocalOptimizationLayout::MyGraphLocalOptimizationLayout(void)
{
	mDistWeightRatio = -1.f;
	mEnergyDecreaseRationLimit = 1e-8;
	mLastEnergy = -1.f;
	mCurrentEnergy = -1.f;
}


MyGraphLocalOptimizationLayout::~MyGraphLocalOptimizationLayout(void)
{
}

void MyGraphLocalOptimizationLayout::SetDistanceEdgeWeightRatio(float ratio){
	mDistWeightRatio = ratio;
}

float MyGraphLocalOptimizationLayout::GetDistanceEdgeWeightRatioHint(const MyBoundingBox& box) const{
	if(mFlat){
		return sqrtf(box.GetFrontFaceArea()/mGraph->GetNumNodes())*100;
	}
	else{
		return powf(box.GetVolume()/mGraph->GetNumNodes(),1.f/3)*100;
	}
}

float MyGraphLocalOptimizationLayout::GetDistanceEdgeWeightRatio() const{
	return mDistWeightRatio;
}

void MyGraphLocalOptimizationLayout::Update(){
	mPos = *mInPos;
	if(this->mFlat){
		this->flatThings();
	}
	if(mDistWeightRatio<=0){
		float hint = this->GetDistanceEdgeWeightRatioHint(this->GetBoundingBox());
		this->SetDistanceEdgeWeightRatio(hint);
	}
	mIteration = 0;
	mLastEnergy = -1.f;
	this->init();
	mCurrentEnergy = energy();
	if(mGraph->GetNumNodes() == 1){
		mPos.clear();
		mPos.push_back(mBoundingBox.GetCenter());
	}
}

bool MyGraphLocalOptimizationLayout::Advance(){
	if(this->ShouldContinue()){
		// update old energy
		mLastEnergy = mCurrentEnergy;

		// advance
		std::vector<MyVec3f> oldPos = mPos;
		for(int i = 0;i<mGraph->GetNumNodes();i++){
			MyVec3f newPos = MyVec3f::zero();

			for(int j = 0;j<mGraph->GetNumNodes();j++){
				if(i != j){
					float oldDist = (oldPos[i]-oldPos[j]).norm();
					if(oldDist !=0){
						MyVec3f jSum = normConst(i,j)*(oldPos[j]+dist(i,j)*(oldPos[i]-oldPos[j])/oldDist);
						newPos += jSum/normConstSum();
					}
				}
			}
			mPos[i] = newPos;
		}

		// limit to bounding box
		MyBoundingBox box;
		for(int i = 0;i<mGraph->GetNumNodes();i++){
			box.Engulf(mPos[i]);
		}
		
		MyArray3f newPos;
		mBoundingBox.MapPoints(mPos,newPos,box);
		mPos = newPos;
		//MyVec3f newCenter = box.GetCenter();
		//for(int i = 0;i<mGraph->GetNumNodes();i++){
		//	mPos[i] += newCenter-mBoundingBox.GetCenter();
		//}
		//for(int i = 0;i<mGraph->GetNumNodes();i++){
		//	mPos[i] = mBoundingBox.BoundPoint(mPos[i]);
		//}

		// update current energy
		mCurrentEnergy = energy();
		mIteration ++;
		return true;
	}
	return false;
}

	
bool MyGraphLocalOptimizationLayout::ShouldContinue() const{
	if(MyGraphIterativeLayout::ShouldContinue()){
		if(mCurrentEnergy == 0) return false;

		// not started yet;
		if(mLastEnergy < 0) return true;

		float energyDecreaseRation = (mLastEnergy-mCurrentEnergy)/mCurrentEnergy;

		if(energyDecreaseRation<0.f) return true;
		else return energyDecreaseRation > mEnergyDecreaseRationLimit;
	}
	else{
		return false;
	}
}

void MyGraphLocalOptimizationLayout::init(){
	int nNodes = mGraph->GetNumNodes();

	// for distance matrix
	
	//// use weight inverse as distance
	//MyMatrixf distMat(nNodes,nNodes);
	//for(int i = 0;i<nNodes;i++){
	//	for(int j = 0;j<nNodes;j++){
	//		if(i != j){
	//			int edgeIdx = mGraph->GetEdgeIndex(MyVec2i(i,j));
	//			if(edgeIdx >=0){
	//				float edgeWeight = mGraph->GetEdgeWeight(i);
	//				if(edgeWeight == 0){
	//					distMat.At(i,j) = MAXDIST;
	//				}
	//				else{
	//					distMat.At(i,j) = 1.f/std::fabs(edgeWeight)*mDistWeightRatio;
	//				}
	//			}
	//			else{
	//				// no connection
	//				distMat.At(i,j) = MAXDIST;
	//			}
	//		}
	//		else distMat.At(i,j) = 0.f;
	//	}
	//}
	//mDistMat = distMat;
	
	// use actual 3d distance
	MyMatrixf distMat(nNodes, nNodes);
	for (int i = 0; i<nNodes; i++){
		for (int j = 0; j<nNodes; j++){
			distMat.At(i, j) = (mInPos->at(i)-mInPos->at(j)).norm()*120;
		}
	}
	mDistMat = distMat;
	

	// for normConstMat;
	MyMatrixf normConstMat(nNodes,nNodes);
	for(int i = 0;i<nNodes;i++){
		for(int j = 0;j<nNodes;j++){
			if(i != j){
				float distance = dist(i,j);
				normConstMat.At(i,j) = 1.f/distance/distance;
			}
			else{
				normConstMat.At(i,j) = 1;
			}
		}
	}
	mNormConstMat = normConstMat;

	// for norm const sum
	mNormConstSum = 0.f;
	for(int i = 0;i<nNodes;i++){
		for(int j = 0;j<nNodes;j++){
			if(i != j){
				mNormConstSum += normConst(i,j);
			}
		}
	}
}

float MyGraphLocalOptimizationLayout::normConst(int i, int j) const{
	return mNormConstMat.At(i,j);
}
float MyGraphLocalOptimizationLayout::dist(int i, int j) const{
	return mDistMat.At(i,j);
}

float MyGraphLocalOptimizationLayout::normConstSum() const{
	return mNormConstSum;
}

float MyGraphLocalOptimizationLayout::energy() const{
	float energy = 0.f;
	for(int i = 0;i<mGraph->GetNumNodes();i++){
		for(int j = i+1;j<mGraph->GetNumNodes();j++){
			float diff = (mPos[i]-mPos[j]).norm()-dist(i,j);
			energy += normConst(i,j)*diff*diff;
		}
	}
	return energy;
}


void  MyGraphLocalOptimizationLayout::flatThings(){
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

	mBoundingBox.SquashDimension(2);
}