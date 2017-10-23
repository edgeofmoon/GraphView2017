#include "MyGraphProjectionLayout.h"

#include "MyGenericHierarchyOverlay.h"

MyGraphProjectionLayout::MyGraphProjectionLayout(void)
{
	mBoarderSize = 1;
}


MyGraphProjectionLayout::~MyGraphProjectionLayout(void)
{
}

void MyGraphProjectionLayout::layoutSubHierarchy(const MyGenericNode* hierarchy, MyBoundingBox boundingBox){
	for(int iCluster = 0;iCluster<hierarchy->GetNumChildren();iCluster++){
		MyArrayStr children;
		MyGenericHierarchyOverlayStr::AddLeaveObjectToArray(&children,hierarchy->GetChild(iCluster));
		MyBoundingBox box;
		for(int iChild = 0;iChild<children.size();iChild++){
			MyVec3f pos = mPos[children[iChild].ToInt()];
			box.Engulf(pos);
		}
		mClusterBoundingBox << box;
	}
}