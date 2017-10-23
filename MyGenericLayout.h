#pragma once

#include "MyVec.h"
#include "MyBoundingBox.h"
#include "MyQuarternion.h"
#include "MyArray.h"

class MyGenericLayoutSyncer;

class MyGenericLayout
{
public:
	MyGenericLayout(void);
	~MyGenericLayout(void);
	
	friend class MyGenericLayoutSyncer;

	enum PosHint{
		NO_USE,
		USE_AS_INIT,
		USE_AS_FINAL
	};

	virtual void Update();
	
	virtual void SetNodePosUse(PosHint use = USE_AS_FINAL);
	virtual MyVec3f GetNodePos(int i) const;
	virtual MyQuarternion GetNodeOrientation(int i) const;
	virtual int GetNodeAtPos(const MyVec3f& pos) const;
	virtual void SetNodePos(const MyArray3f* pos);
	
	virtual MyBoundingBox GetBoundingBox() const;
	virtual void SetBoundingBox(const MyBoundingBox& box);
	virtual void MapToBoundingBox(const MyBoundingBox& box);

	virtual void SetSyncer(MyGenericLayoutSyncer* syncer);
	virtual MyGenericLayoutSyncer* GetSyncer() const;

	// only the master layout could update
	// will make all slave layouts the same
	virtual void SyncUpdate();

protected:
	// calculated pos
	MyArray3f mPos;

	// calculated orientation
	MyArray<MyQuarternion> mOrientations;

	// user defined input pos
	const MyArray3f* mInPos;

	// how to use
	PosHint mUse;
	
	virtual void updataBoundingBox();
	MyBoundingBox mBoundingBox;

	MyGenericLayoutSyncer* mSyncer;
};

