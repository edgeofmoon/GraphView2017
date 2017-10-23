#pragma once
#include "mymultiview.h"
class MyAntiAliasingView :
	public MyMultiView
{
public:
	MyAntiAliasingView(void);
	~MyAntiAliasingView(void);
	
	virtual void Show();
};

