#include "MyGenericLayoutSyncer.h"

#include "MyGenericLayout.h"

MyGenericLayoutSyncer::MyGenericLayoutSyncer(void)
{
	mLayouts = new MyArray<MyGenericLayout*>;
	mLayout = 0;
}

MyGenericLayoutSyncer::~MyGenericLayoutSyncer(void)
{
	delete mLayouts;
}

void MyGenericLayoutSyncer::Update(MyGenericLayout* layout){
	if(!mLayout){
		if(mLayouts->empty()) return;
		else mLayout = mLayouts->at(0);
	}
	if(layout == mLayout || !layout){
		mLayout->Update();
		//for (int i = 0; i < mLayouts->size(); i++){
		//	mLayouts->at(i)->mPos = mLayout->mPos;
		//	mLayouts->at(i)->mBoundingBox = mLayout->mBoundingBox;
		//}
	}
	else{
		layout->mPos = mLayout->mPos;
		layout->mBoundingBox = mLayout->mBoundingBox;
	}
}

bool MyGenericLayoutSyncer::SyncLayout(MyGenericLayout* layout){
	return mLayouts->PushBackUniqueOne(layout);
}

bool MyGenericLayoutSyncer::DesyncLayout(MyGenericLayout* layout){
	return mLayouts->EraseOne(layout);
}

// the master layout
void MyGenericLayoutSyncer::SetMasterLayout(MyGenericLayout* layout){
	mLayout = layout;
}

const MyGenericLayout* MyGenericLayoutSyncer::GetMasterLayout() const{
	return mLayout;
}

bool MyGenericLayoutSyncer::Connect(MyGenericLayout* layout, MyGenericLayoutSyncer* syncer){
	layout->SetSyncer(syncer);
	return syncer->SyncLayout(layout);
}

bool MyGenericLayoutSyncer::Disconnect(MyGenericLayout* layout, MyGenericLayoutSyncer* syncer){
	layout->SetSyncer(0);
	return syncer->DesyncLayout(layout);
}