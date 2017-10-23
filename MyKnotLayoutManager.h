#pragma once

#include "MyKnot.h"
#include "MyKnotContainer.h"
#include "MyGenericLayout.h"

class MyKnotLayoutManager
{
public:
	MyKnotLayoutManager();
	~MyKnotLayoutManager(void);
	
	virtual void Update();
	virtual void SetKnotContainer(const MyKnotContainer* knotContainer);
	const MyKnot* GetKnot(int i) const;
	int GetNumKnots() const;

protected:
	const MyKnotContainer* mKnotContainer;

	void initLayout();
};

