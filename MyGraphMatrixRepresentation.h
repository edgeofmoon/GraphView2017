#pragma once
#include "mygraphrepresentation.h"
#include "MyColor4.h"

class MyGraphMatrixRepresentation :
	public MyGraphRepresentation
{
public:
	MyGraphMatrixRepresentation(void);
	~MyGraphMatrixRepresentation(void);
	
	virtual void ShowNode(int i);
	virtual void ShowEdge(int i);
	virtual void ShowHighlightedNode(int i);
	virtual void ShowHighlightedEdge(int i);
	virtual void ShowLabels();
	
protected:
	virtual void update();
};

