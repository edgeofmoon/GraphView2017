#include "MyGenericEventInterpreter.h"


MyGenericEventInterpreter::MyGenericEventInterpreter(void)
{
}


MyGenericEventInterpreter::~MyGenericEventInterpreter(void)
{
}

int MyGenericEventInterpreter::InterpretEvent(MyGenericEvent& eve, MyGenericEventListener* lister){
	if(eve.GetEventHandleState() == MyGenericEvent::EVENT_HANDLED){
		return 0;
	}
	else if(lister->IsDeaf()){
		return 0;
	}
	int _handleResult;
	switch(eve.GetEventSource()){
	case MyGenericEvent::MOUSE_MOVE:
		_handleResult = lister->mouseMoveEventHandler(eve);
		return _handleResult;
		break;
	case MyGenericEvent::MOUSE_KEY:
		if(eve.GetEventKeyState() == MyGenericEvent::KEY_DOWN){
			_handleResult = lister->mousePressEventHandler(eve);
			return _handleResult;
			break;
		}
		else{
			_handleResult = lister->mouseReleaseEventHandler(eve);
			return _handleResult;
			break;
		}
	case MyGenericEvent::KEYBOARD_KEY:
		if(eve.GetEventKeyState() == MyGenericEvent::KEY_DOWN){
			_handleResult = lister->keyPressEventHandler(eve);
			return _handleResult;
			break;
		}
		else{
			_handleResult = lister->keyReleaseEventHandler(eve);
			return _handleResult;
			break;
		}
	case MyGenericEvent::TIMER:
		_handleResult = lister->idleEventHandler(eve);
		return _handleResult;
		break;
	case MyGenericEvent::WINDOW_RESIZE:
		_handleResult = lister->windowResizeEventHandler(eve);
		return _handleResult;
		break;
	default:
		break;
	}
	return 0;
}