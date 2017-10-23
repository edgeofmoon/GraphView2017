#pragma once

#include "MyGenericEvent.h"

class MyGenericEventListener
{
public:
	MyGenericEventListener(void);
	virtual ~MyGenericEventListener(void);
	
	friend class MyGenericEventInterpreter;

	int EventHandler(MyGenericEvent& eve);

	enum MyListenerStatus{
		LISTENER_READY = 0,
		LISTENER_DEAF = 1,
		LISTENER_ANSWERING = 2
	};

	void StartToAnswer();
	void ReadyToListen();
	void DonotListen();

	bool IsAnswering() const;
	bool IsDeaf() const;

protected:
	virtual int keyReleaseEventHandler(MyGenericEvent& eve){
		return 0;
	};
	virtual int keyPressEventHandler(MyGenericEvent& eve){
		return 0;
	};
	virtual int mousePressEventHandler(MyGenericEvent& eve){
		return 0;
	};
	virtual int mouseReleaseEventHandler(MyGenericEvent& eve){
		return 0;
	};
	virtual int mouseMoveEventHandler(MyGenericEvent& eve){
		return 0;
	};
	virtual int idleEventHandler(MyGenericEvent& eve){
		return 0;
	}
	virtual int windowResizeEventHandler(MyGenericEvent& eve){
		return 0;
	}


	MyListenerStatus mListenerStatus;
};

/*
#define MYMACRO_DEFINE_ALL_EMPTY_EVENT_HANDLER \
	virtual int keyReleaseEventHandler(unsigned char, int, int){ \
		return 0; \
	}; \
	virtual int keyPressEventHandler(unsigned char, int, int){ \
		return 0; \
	}; \
	virtual int mousePressEventHandler(int, int, int){ \
		return 0; \
	}; \
	virtual int mouseReleaeEventHandler(int, int, int){ \
		return 0; \
	}; \
	virtual int mouseMoveEventHandler(int, int){ \
		return 0; \
	};
*/