#pragma once
#include "mygraphiterativelayout.h"
#include "MyGenericGraph.h"
#include "MyMatrix.h"

/* reference:
     http://www.graphviz.org/Documentation/GKN04.pdf
*/

class MyGraphLocalOptimizationLayout :
	public MyGraphIterativeLayout
{
public:
	MyGraphLocalOptimizationLayout(void);
	~MyGraphLocalOptimizationLayout(void);

	void SetDistanceEdgeWeightRatio(float ratio);

	float GetDistanceEdgeWeightRatioHint(const MyBoundingBox& box) const;
	float GetDistanceEdgeWeightRatio() const;
	
	virtual void Update();

	virtual bool Advance();

	
	virtual bool ShouldContinue() const;

protected:
	void init();
	float normConst(int i, int j) const;
	float dist(int i, int j) const;

	float normConstSum() const;

	float energy() const;
	float mLastEnergy;
	float mCurrentEnergy;
	float mEnergyDecreaseRationLimit;
	
	void flatThings();

	// this implementation only suits dense graph
	// should replace them with generic graph class
	float mDistWeightRatio;
	MyMatrixf mDistMat;
	MyMatrixf mNormConstMat;
	float mNormConstSum;

};

