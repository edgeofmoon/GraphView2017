#pragma once
#include "MyGraphIterativeLayout.h"

#include <vector>

class MyGraphFruchtermanReingoldLayout :
	public MyGraphIterativeLayout
{
public:
	MyGraphFruchtermanReingoldLayout(void);
	~MyGraphFruchtermanReingoldLayout(void);
	virtual void Update();

	
	void LimitTemperature(float temp);

	// advance an iteration until cools down
	// return false if totally cool
	virtual bool Advance();
	virtual bool ShouldContinue() const;
	
protected:
	void flatThings();
	void initLayout();
	void cool();

	const float repulsiveForce(float distance, float k = 1.f) const;
	const float attractiveForce(float distance, float k = 1.f) const;

	std::vector<MyVec3f> mDisplacements;
	
	//layour parameter
	float mForceParameter;
	float mTemperature;

	float mTemperatureLimit;

};

