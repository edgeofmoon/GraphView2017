#pragma once
#include "mygraphlayout.h"

class MyGraphIterativeLayout :
	public MyGraphLayout
{
public:
	MyGraphIterativeLayout(void);
	~MyGraphIterativeLayout(void);

	// set iteraction limits
	virtual void LimitIterations(int n);

	// if could continue return true
	// else return false
	virtual bool Advance();

	// advance until if returns false
	virtual void AdvanceToFinal();

	// the base class only judge if iteraction limit
	// is reached.
	virtual bool ShouldContinue() const;

protected:
	int mIteration;

	int mIterationLimit;
};

