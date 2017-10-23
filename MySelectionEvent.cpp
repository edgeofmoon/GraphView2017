#include "MySelectionEvent.h"
#include "MyGenericHierarchyOverlay.h"
#include "MyScene.h"
#include "MyGraphKnot.h"
MySelectionEvent::MySelectionEvent(const MyGenericEvent& eve)
{	
	mEventSource = eve.GetEventSource();
	mHandleState = eve.GetEventHandleState();
	mEventKeyState = eve.GetEventKeyState();
	mKeyboardKey = eve.GetEventKeyboardKey();
	mMouseX = eve.GetMouseX();
	mMouseY = eve.GetMouseY();
	mEventMouseKey = eve.GetEventMouseKey();
	mEventIndex = eve.GetEventIndex();
	mNeedRedraw = eve.DoNeedRedraw();
}


MySelectionEvent::~MySelectionEvent(void)
{
}

MyString MySelectionEvent::GetActionName() const{
	return mActionName;
}

MyString MySelectionEvent::GetActionValue() const{
	return mActionValue;
}

void MySelectionEvent::SetActionName(const MyString& str){
	mActionName = str;
}

void MySelectionEvent::SetActionValue(const MyString& str){
	mActionValue = str;
}

void MySelectionEvent::SetAction(const MyString& name,const MyString& value){
	this->SetActionName(name);
	this->SetActionValue(value);
}

MyString MySelectionEvent::GetSelectionActionName(MySelectionAction selAct){
	int sel = (int)selAct;
	MyString names[] = {
		"NONE",
		"ADDD",
		"DELE",
		"REPL",
		"LKUP"
	};
	return names[sel];
}

void MySelectionEvent::SetActionName(const MySelectionAction& sel){
	this->SetActionName(MySelectionEvent::GetSelectionActionName(sel));
}

void MySelectionEvent::SetAction(const MySelectionAction& name,const MyString& value){
	this->SetActionName(name);
	this->SetActionValue(value);
}

void MySelectionEvent::DeriveActionFromKnot(const MyKnot* knot, const MyGenericNode* newSelection){
	mActionName = GetSelectionActionName(SEL_NONE);
	mActionValue = MyString();
	if(!knot) return;
	// unfinished
}

void MySelectionEvent::DeriveActionFromSelection(const MySelectionAction& selAct, const MyGenericNode* newSelection, const MyScene* scene){
	this->SetActionName(selAct);
	mActionValue = MyString();
	if(!newSelection) return;
	const MyGenericHierarchyOverlayStr* sel = 
		dynamic_cast<const MyGenericHierarchyOverlayStr*>(newSelection);
	if(!sel) return;
	MyGenericHierarchyOverlayStr node("node");
	MyGenericHierarchyOverlayStr edge("edge");
	if(MyGenericHierarchyOverlayStr::IsObjectEqual(sel,&node)
		||MyGenericHierarchyOverlayStr::IsObjectEqual(sel,&edge)){
		mActionValue = sel->GetObject();
		for(int i = 0;i<sel->GetNumChildren();i++){
			mActionValue += MyString(" ");
			mActionValue += dynamic_cast<const MyGenericHierarchyOverlayStr*>
				(sel->GetChild(i))->GetObject();
		}
	}
	else if (scene){
		const MyGraphKnot* gknot = dynamic_cast<const MyGraphKnot*>(scene->GetKnot(0));
		mActionValue = "cluster";
		for(int ic = 0;ic < gknot->GetHierarchy()->GetNumChildren();ic++){
			if(MyGenericHierarchyOverlayStr::IsObjectEqual(gknot->GetHierarchy()->GetChild(ic),sel)){
				mActionValue += MyString(" ");
				//mActionValue += MyString(ic);
				mActionValue += sel->GetObject();
			}
		}
	}
}

MySelectionEvent MySelectionEvent::GenerateKeyboardKeyEvent(unsigned char c, MyKeyState state, int x, int y){
	MyGenericEvent eve = MyGenericEvent::GenerateKeyboardKeyEvent(c,state,x,y);
	return MySelectionEvent(eve);
}

MySelectionEvent MySelectionEvent::GenerateMouseKeyEvent(MyMouseKey button, MyKeyState state, int x, int y){
	MyGenericEvent eve = MyGenericEvent::GenerateMouseKeyEvent(button,state,x,y);
	return MySelectionEvent(eve);
}

MySelectionEvent MySelectionEvent::GenerateMouseMoveEvent(MyMouseKey button, MyKeyState state, int x, int y){
	MyGenericEvent eve = MyGenericEvent::GenerateMouseMoveEvent(button,state,x,y);
	return MySelectionEvent(eve);
}

MySelectionEvent MySelectionEvent::GenerateTimerEvent(){
	MyGenericEvent eve = MyGenericEvent::GenerateTimerEvent();
	return MySelectionEvent(eve);
}

MySelectionEvent MySelectionEvent::GenerateWindowResizeEvent(int x, int y){
	MyGenericEvent eve = MyGenericEvent::GenerateWindowResizeEvent(x, y);
	return MySelectionEvent(eve);
}