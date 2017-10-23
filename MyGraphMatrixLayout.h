#pragma once

#include "MyVec.h"
#include "MyGraphLayout.h"
#include "MyGenericNode.h"

class MyGraphMatrixLayout
	:public MyGraphLayout
{
public:
	MyGraphMatrixLayout(void);
	~MyGraphMatrixLayout(void);

	
	virtual void Update();

	// exchange CURRENT Pos
	void ExchangPos(int i, int j);
	// set current Pos oldPos nodes= to newPos
	// nodes Pos before max(oldPos, newPos)
	// or after min(oldPos, newPos) don't change
	// if oldPos>newPos, node Pos between (newPos, oldPos)
	// upgrade Pos by 1; otherwise downgrade by 1
	//void SetPos(int oldPos, int newPos);

	void SetNodeOrder(const std::vector<int>& order);

	void OrderByHierarchy(const MyGenericNode* hierarchy);

	const MyGenericNode* GetHierarchy() const;

protected:
	const MyGenericNode* mHierarchy;
	void initPos();
	void orderByHierarchy(const MyGenericNode* hierarchy, MyVec3f& pos, const MyVec3f& offset);
};

