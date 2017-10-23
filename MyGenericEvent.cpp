#include "MyGenericEvent.h"

int MyGenericEvent::windowWidth = 1024;
int MyGenericEvent::windowHeight = 768;
int MyGenericEvent::lastestEventIndex = 0;

MyGenericEvent::MyGenericEvent(void)
{
	mEventSource = NONE_EVENT;
	mHandleState = EVENT_NO_RESPONSE;
	mNeedRedraw = false;
}

// key press or release
MyGenericEvent::MyGenericEvent(unsigned char c, MyKeyState state, int x, int y, MyEventSource source, int eveIdx)
{
	mEventSource = source;
	mEventKeyState = state;
	mKeyboardKey = c;
	mMouseX = x;
	mMouseY = y;
	mEventMouseKey = MOUSE_KEY_INVALID;
	mEventIndex = eveIdx;
	mHandleState = EVENT_NO_RESPONSE;
	mNeedRedraw = false;
}

// mouse press or release
MyGenericEvent::MyGenericEvent(MyMouseKey button, MyKeyState state, int x, int y, MyEventSource source, int eveIdx)
{
	mEventSource = source;
	mEventKeyState = state;
	mKeyboardKey = '\0';
	mEventMouseKey = button;
	mMouseX = x;
	mMouseY = y;
	mEventIndex = eveIdx;
	mHandleState = EVENT_NO_RESPONSE;
	mNeedRedraw = false;
}

// resize event
MyGenericEvent::MyGenericEvent(int x, int y, MyEventSource source, int eveIdx){
	windowWidth = x;
	windowHeight = y;
	mEventSource = source;
	mEventIndex = eveIdx;
	mHandleState = EVENT_NO_RESPONSE;
	mNeedRedraw = false;
}
	// timer event
MyGenericEvent::MyGenericEvent(MyEventSource source, int eveIdx){
	mEventSource = source;
	mEventIndex = eveIdx;
	mHandleState = EVENT_NO_RESPONSE;
	mNeedRedraw = false;
}

MyGenericEvent MyGenericEvent::GenerateKeyboardKeyEvent(unsigned char c, MyKeyState state, int x, int y){
	int idx = MyGenericEvent::generateNewEventIndex();
	return MyGenericEvent(c, state, x, y,KEYBOARD_KEY, idx);
}

MyGenericEvent MyGenericEvent::GenerateMouseKeyEvent(MyMouseKey button, MyKeyState state, int x, int y){
	int idx = MyGenericEvent::generateNewEventIndex();
	return MyGenericEvent(button, state, x, y,MOUSE_KEY, idx);
}

MyGenericEvent MyGenericEvent::GenerateMouseMoveEvent(MyMouseKey button, MyKeyState state, int x, int y){
	int idx = MyGenericEvent::generateNewEventIndex();
	return MyGenericEvent(button, state, x, y,MOUSE_MOVE, idx);
}

MyGenericEvent MyGenericEvent::GenerateTimerEvent(){
	int idx = MyGenericEvent::generateNewEventIndex();
	return MyGenericEvent(TIMER, idx); 
}

MyGenericEvent MyGenericEvent::GenerateWindowResizeEvent(int x, int y){
	int idx = MyGenericEvent::generateNewEventIndex();
	return MyGenericEvent(x, y, WINDOW_RESIZE, idx); 
}

MyGenericEvent::~MyGenericEvent(void)
{
}

int MyGenericEvent::generateNewEventIndex(){
	return ++lastestEventIndex;
}