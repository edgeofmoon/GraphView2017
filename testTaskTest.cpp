#include "MyGraphTaskInstance.h"
#include "MyGraphicsTool.h"
#include "MyArray.h"
#include "MyUtility.h"
#include "MyGraphTaskUI.h"
#include "MyScene.h"
#include "MyMultiView.h"
#include "MyGraphClusterVariationTask.h"
#include "MySelectionEvent.h"
#include "VisConfig.h"

#define TASKSET 1

typedef MySelectionEvent MyEvent;

#include <fstream>

std::ostream* logFile;
std::ostream* interactionLogFile;
MyGraphicsTool helper;
MyGraphTaskInstance* task = 0;

bool isBlinking = false;
clock_t blinkStarts;
float blinkTime = 0.1;

bool checkFile = false;
int NUMTESTS = 648;
int NUMTASKS = 4;
int NUMTRAINING = 27;
int NUMTRAILS = NUMTESTS/3;
int taskIdx = 13;
int pIdx = 3;
// 0 study
// else training
int mode = 1;

MyArrayStr taskConfigs;
MyArrayi taskOrder;

// suppose only one keyboard or mouse
MyGenericEvent::MyMouseKey lastEventMouseKey = MyGenericEvent::MOUSE_KEY_LEFT;
MyGenericEvent::MyKeyState mouseKeyState = MyGenericEvent::KEY_UP;

//#include "gl/glut.h"

MyGenericEvent::MyMouseKey toKey(int k){
	return (k == 0 ?
		MyGenericEvent::MOUSE_KEY_LEFT : (k == 1 ?
		MyGenericEvent::MOUSE_KEY_MIDDLE : MyGenericEvent::MOUSE_KEY_RIGHT));
}

MyGenericEvent::MyKeyState toState(int s){
	return (s == 0 ? MyGenericEvent::KEY_DOWN : MyGenericEvent::KEY_UP);
}

bool emptyShown = false;

void NextTask(){
	SafeFreeObject(task);
	if(taskIdx == taskOrder.size()+1){
		logFile->flush();
		interactionLogFile->flush();
		exit(1);
	}
	else if(taskIdx == taskOrder.size()){
		task = new MyGraphTaskInstance(taskIdx-1, NUMTRAILS);
		task->SetParticipantIndex(pIdx);
		MyString modeString;
		if(mode == 0){
			modeString = "You have finish the study. Thank you!";
		}
		if(mode == 1){
			modeString = "Thank you. You have finished the training.";
		}
		task->GetTaskUI()->SetWarningText(modeString);
		task->GetTaskUI()->SetFinishBoxString("FINISH");
		isBlinking = false;
		taskIdx ++;
	}
	else if (taskIdx % (NUMTRAILS / NUMTASKS) == 0 && !emptyShown){
		emptyShown = true;
		task = new MyGraphTaskInstance(taskIdx-1, NUMTRAILS);
		task->SetParticipantIndex(pIdx);
		MyGraphTaskInstance* peek = new MyGraphTaskInstance(taskIdx, NUMTRAILS);
		peek->SetParticipantIndex(pIdx);
		peek->LoadFromFile(taskConfigs[taskOrder[taskIdx]]);
		task->GetTaskUI()->SetWarningText(peek->GetTask()->GetTaskDescription());
		task->GetTaskUI()->SetFinishBoxString("START");
		SafeFreeObject(peek);
		isBlinking = false;
	}
	else{
		emptyShown = false;
		task = new MyGraphTaskInstance(taskIdx, NUMTRAILS);
		task->SetParticipantIndex(pIdx);
		task->LoadFromFile(taskConfigs[taskOrder[taskIdx]]);
		if (task->GetTask()->DoNeedPreHint()){
			blinkStarts = clock();
			isBlinking = true;
		}
		else{
			isBlinking = false;
		}
		taskIdx++;
	}
	if(mode == 1){
		task->GetTaskUI()->SetResultShown(true);
	}
	else{
		task->GetTaskUI()->SetResultShown(false);
	}
	/*
	if(mode != 0 && taskIdx==2){
		task->GetScene()->SetInteractionMode(1);
	}
	else{
		task->GetScene()->SetInteractionMode(0);
	}
	*/
}

void DisplayFunc(void){
	helper.ClearFrameBuffer();
	
	if(isBlinking){
		double c_time = task->GetTimer().GetCurrentTime();
		task->SetShowMode(1);
		if((int)(c_time*10)%7<3.5){
			task->Show();
		}
		else{
			MyMultiView* view = dynamic_cast<MyMultiView*>(task->GetView());
			if(view){
				for(int i = 0;i<task->GetScene()->GetNumKnots();i++){
					view->DrawClipPlanes(i);
				}
			}
		}
		helper.FreshScreen();
		if(c_time-blinkStarts/(double) CLOCKS_PER_SEC > blinkTime){
			isBlinking = false;
		}
	}
	else{
		task->SetShowMode(0);
		task->Show();
		helper.FreshScreen();
		task->GetTimer().StartTimerIfNotYet();
		task->LogTrialStartIfHavent(*interactionLogFile);
		task->GetTaskUI()->SetEnable(true);
	}
	
}
void ReshapeFunc(int w, int h){
	MyEvent eve = MyEvent::GenerateWindowResizeEvent(w, h);
	task->EventHandler(eve);
}
void KeyReleaseFunc(unsigned char c, int x, int y){
	MyEvent eve = MyEvent::GenerateKeyboardKeyEvent(c, MyEvent::KEY_UP, x, y);
	helper.Update();
}
void KeyPressFunc(unsigned char c, int x, int y){
	if(c == 27){
		exit(0);
	}
	MyEvent eve = MyEvent::GenerateKeyboardKeyEvent(c, MyEvent::KEY_DOWN, x, y);
	task->EventHandler(eve);
	helper.Update();
}
void MouseKeyFunc(int button, int state, int x, int y){
	lastEventMouseKey = toKey(button);
	mouseKeyState = toState(state);
	if(isBlinking) return;
	MyEvent eve = MyEvent::GenerateMouseKeyEvent(lastEventMouseKey, mouseKeyState, x, y);
	int rst = task->EventHandler(eve);
	task->LogSelection(*interactionLogFile, eve);
	if(rst == 2){
		task->GetTimer().FreshEndTime();
		task->Log(*logFile);
		NextTask();
	}
	else if(rst == 4){
		if(task->GetTaskUI()->IsPaused()){
			task->GetTimer().PauseTimer();
			//task->LogPauseOrResume(*interactionLogFile, true);
		}
		else{
			task->GetTimer().ResumeTimer();
			//task->LogPauseOrResume(*interactionLogFile, false);
		}
	}
	//task->GetTaskUI()->UpdateUserAnswerFromGraphKnot();
	helper.Update();
}

int lastMoveX = -1;
int lastMoveY = -1;
void MouseMoveFunc(int x, int y){
	if(x == lastMoveX && y == lastMoveY){
		// do not handle event when mouse does not move
		return;
	}
	if(isBlinking) return;
	MyEvent eve = MyEvent::GenerateMouseMoveEvent(
		lastEventMouseKey, MyGenericEvent::KEY_DOWN, x, y);
	task->EventHandler(eve);
	task->LogSelection(*interactionLogFile, eve);
	if (eve.DoNeedRedraw()){
		helper.Update();
	}
	lastMoveX = x;
	lastMoveY = y;
}

void MouseMovePassiveFunc(int x, int y){
	if(x == lastMoveX && y == lastMoveY){
		// do not handle event when mouse does not move
		return;
	}
	if(isBlinking) return;
	MyEvent eve = MyEvent::GenerateMouseMoveEvent(
		lastEventMouseKey, MyGenericEvent::KEY_UP, x, y);
	task->EventHandler(eve);
	task->LogSelection(*interactionLogFile, eve);
	if (eve.DoNeedRedraw()){
		helper.Update();
	}
	lastMoveX = x;
	lastMoveY = y;
}

void IdleFunc(){
	/*
	MyGenericEvent eve = MyGenericEvent::GenerateTimerEvent();
	//task->EventHandler(eve);
	if (eve.DoNeedRedraw()){
		helper.Update();
	}
	*/
	static bool _blink = true;
	if(_blink || isBlinking){
		helper.Update();
	}
	_blink = isBlinking;
}

void checkFileExist(const MyString& fileName){
	if(!checkFile) return;
	std::ifstream infile;
	infile.open(fileName);
	if(infile.is_open()){
		std::cerr << "Log file "<< fileName << " already exists. Please handle!\n";
		MyString t;
		std::getline(std::cin,t,'\n');
		infile.close();
		exit(0);
	}
}

int main(int argc, char* argv[]){
	
	if(argc>1){
		taskIdx = atoi(argv[1]);
	}
	if(argc >2){
		pIdx = atoi(argv[2]);
	}
	if(argc > 3){
		mode = atoi(argv[3]);
	}

	helper.Init(&argc,argv);
	
	std::ofstream log;
	std::ofstream log2;

	log.open("test.txt");
	log2.open("test_selection.txt");
	logFile = &log;
	interactionLogFile = &log2;

	NUMTRAILS = NUMTRAINING;
	MyString dropBoxFolder = "test3\\";
	unsigned randSeed = 0;
	if(argc > 1){
		randSeed = atoi(argv[1]);
	}

	//taskConfigs<<dropBoxFolder+MyString("test")+".txt";
	for (int i = 0; i<NUMTRAINING; i++){
		taskConfigs << dropBoxFolder+MyString(i)+".txt";
	}
	taskOrder = MyArrayi::GetSequence(0,NUMTRAINING-1);

	NextTask();

	helper.RegisterDisplayFunction(DisplayFunc);
	helper.RegisterReshapeFunction(ReshapeFunc);
	helper.RegisterKeyReleaseFunction(KeyReleaseFunc);
	helper.RegisterKeyPressFunction(KeyPressFunc);
	helper.RegisterMouseKeyFunction(MouseKeyFunc);
	helper.RegisterMouseMoveFunction(MouseMoveFunc);
	//helper.RegisterMousePassiveMoveFunction(MouseMovePassiveFunc);
	helper.RegisterIdleFunction(IdleFunc);
	//helper.SetFullScreen();
	helper.Start();

	log.close();
	log2.close();
	return 1;
}