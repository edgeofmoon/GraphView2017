#pragma once
#include "mygenericevent.h"
#include "MyGenericNode.h"
#include "MyString.h"
#include "MyKnot.h"

class MyScene;
class MySelectionEvent :
	public MyGenericEvent
{
public:
	MySelectionEvent(const MyGenericEvent& eve);
	~MySelectionEvent(void);

	enum MySelectionAction{
		SEL_NONE = 0,
		SEL_ADD = 1,
		SEL_DELETE = 2,
		SEL_REPLACE = 3,
		SEL_LOOKUP = 4
	};

	static MyString GetSelectionActionName(MySelectionAction selAct);

	virtual void SetActionName(const MySelectionAction& sel);
	virtual void SetActionName(const MyString& str);
	virtual void SetActionValue(const MyString& str);
	virtual void SetAction(const MyString& name,const MyString& value);
	virtual void SetAction(const MySelectionAction& name,const MyString& value);

	virtual MyString GetActionName() const;
	virtual MyString GetActionValue() const;

	virtual void DeriveActionFromKnot(const MyKnot* knot, const MyGenericNode* newSelection);
	virtual void DeriveActionFromSelection(const MySelectionAction& sel, const MyGenericNode* newSelection, const MyScene* scene = 0);

	// opengl interface
	static MySelectionEvent GenerateKeyboardKeyEvent(unsigned char c, MyKeyState state, int x, int y);
	static MySelectionEvent GenerateMouseKeyEvent(MyMouseKey button, MyKeyState state, int x, int y);
	static MySelectionEvent GenerateMouseMoveEvent(MyMouseKey button, MyKeyState state, int x, int y);
	static MySelectionEvent GenerateTimerEvent();
	static MySelectionEvent GenerateWindowResizeEvent(int x, int y);
protected:
	MyString mActionName;
	MyString mActionValue;
};

