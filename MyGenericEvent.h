#pragma once

class MyGenericEvent
{
public:
	MyGenericEvent(void);

	enum MyEventSource{
		NONE_EVENT,
		MOUSE_MOVE,
		MOUSE_KEY,
		KEYBOARD_KEY,

		// multiple handlers allowed
		TIMER,
		WINDOW_RESIZE
	};

	enum MyMouseKey{
		MOUSE_KEY_INVALID = -1,
		MOUSE_KEY_LEFT = 0,
		MOUSE_KEY_MIDDLE = 1,
		MOUSE_KEY_RIGHT = 2
	};

	enum MyKeyState{
		KEY_STATE_INVALID = -1,
		KEY_DOWN = 0,
		KEY_UP = 1
	};

	// state can only progess, not reverse
	enum MyHandleState{
		EVENT_NO_RESPONSE = 0,
		EVENT_VISITED,
		EVENT_HANDLED
	};

	// opengl interface
	static MyGenericEvent GenerateKeyboardKeyEvent(unsigned char c, MyKeyState state, int x, int y);
	static MyGenericEvent GenerateMouseKeyEvent(MyMouseKey button, MyKeyState state, int x, int y);
	static MyGenericEvent GenerateMouseMoveEvent(MyMouseKey button, MyKeyState state, int x, int y);
	static MyGenericEvent GenerateTimerEvent();
	static MyGenericEvent GenerateWindowResizeEvent(int x, int y);

	~MyGenericEvent(void);

	// visited event could be handled and visited again
	// state can only progess, not reverse
	virtual void Visited(){
		if(mHandleState == EVENT_NO_RESPONSE){
			mHandleState = EVENT_VISITED;
		}
	}

	// handled event could only be visited
	virtual void Handled(){
		mHandleState = EVENT_HANDLED;
	}

	virtual void SetEventHandleState(MyHandleState state){
		mHandleState = state;
	}

	virtual MyHandleState GetEventHandleState() const{
		return mHandleState;
	}
	virtual MyEventSource GetEventSource() const{
		return mEventSource;
	}
	virtual MyMouseKey GetEventMouseKey() const{
		return mEventMouseKey;
	}
	virtual MyKeyState GetEventKeyState() const{
		return mEventKeyState;
	}
	virtual char GetEventKeyboardKey() const{
		return mKeyboardKey;
	}
	virtual int GetMouseX() const{
		return mMouseX;
	}
	virtual int GetMouseY() const{
		return mMouseY;
	}
	static int GetWindowWidth(){
		return windowWidth;
	}
	static int GetWindowHeight(){
		return windowHeight;
	}
	virtual int GetEventIndex() const{
		return mEventIndex;
	}
	static int GetLatestEventIndex(){
		return lastestEventIndex;
	}
	virtual void SetNeedRedraw(bool needRedraw = true){
		mNeedRedraw = needRedraw;
	}
	virtual bool DoNeedRedraw() const{
		return mNeedRedraw;
	}
protected:
	MyEventSource mEventSource;
	MyMouseKey mEventMouseKey;
	MyKeyState mEventKeyState;
	char mKeyboardKey;
	int mMouseX, mMouseY;

	static int generateNewEventIndex();
	int mEventIndex;

	static int windowWidth, windowHeight;
	static int lastestEventIndex;

	// key press or release
	MyGenericEvent(unsigned char c, MyKeyState state, int x, int y, MyEventSource source, int eveIdx);
	// mouse press or release or move
	MyGenericEvent(MyMouseKey button, MyKeyState state, int x, int y, MyEventSource source, int eveIdx);
	// resize event
	MyGenericEvent(int x, int y, MyEventSource source, int eveIdx);
	// timer event
	MyGenericEvent(MyEventSource source, int eveIdx);

	MyHandleState mHandleState;
	// should add a time stamp?

	bool mNeedRedraw;
};

