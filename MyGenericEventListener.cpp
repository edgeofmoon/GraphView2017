#include "MyGenericEventListener.h"
#include "MyGenericEventInterpreter.h"

MyGenericEventListener::MyGenericEventListener(void)
{
	this->ReadyToListen();
}


MyGenericEventListener::~MyGenericEventListener(void)
{
}

int MyGenericEventListener::EventHandler(MyGenericEvent& eve){
	return MyGenericEventInterpreter::InterpretEvent(eve,this);
	return 0;
}
void MyGenericEventListener::StartToAnswer(){
	mListenerStatus = MyGenericEventListener::LISTENER_ANSWERING;
}

void MyGenericEventListener::ReadyToListen(){
	mListenerStatus = MyGenericEventListener::LISTENER_READY;
}
void MyGenericEventListener::DonotListen(){
	mListenerStatus = MyGenericEventListener::LISTENER_DEAF;
}

bool MyGenericEventListener::IsAnswering() const{
	return mListenerStatus == MyGenericEventListener::LISTENER_ANSWERING;
}
bool MyGenericEventListener::IsDeaf() const{
	return mListenerStatus == MyGenericEventListener::LISTENER_DEAF;
}