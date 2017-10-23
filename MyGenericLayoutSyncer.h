#pragma once

#include "MyArray.h"

class MyGenericLayout;

class MyGenericLayoutSyncer
{
public:
	MyGenericLayoutSyncer(void);
	~MyGenericLayoutSyncer(void);

	void Update(MyGenericLayout* layout = 0);

	bool SyncLayout(MyGenericLayout* layout);
	bool DesyncLayout(MyGenericLayout* layout);

	// the master layout
	void SetMasterLayout(MyGenericLayout* layout);
	const MyGenericLayout* GetMasterLayout() const;

	static bool Connect(MyGenericLayout* layout, MyGenericLayoutSyncer* syncer);
	static bool Disconnect(MyGenericLayout* layout, MyGenericLayoutSyncer* syncer);

protected:
	MyArray<MyGenericLayout*>* mLayouts;
	
	// the master layout
	MyGenericLayout* mLayout;
};

