#pragma once

#include "MyGenericEvent.h"
#include "MyGenericEventListener.h"

class MyGenericEventInterpreter
{
public:
	MyGenericEventInterpreter(void);
	~MyGenericEventInterpreter(void);

	static int InterpretEvent(MyGenericEvent& eve,
		MyGenericEventListener* lister);
};

