#include "MyGraphTaskInstance.h"
#include "MyArray.h"
#include "MyUtility.h"
#include "MyGraphChoiceTask.h"

#include <fstream>

#include <GL/freeglut.h>

std::ofstream rstFile;
MyGraphTaskInstance* task = 0;

bool isBlinking = false;
clock_t blinkStarts;
float blinkTime = 2;

int NUMTESTS = 324;
int NUMTRAILS = 108;
int NUMTRAINING = 36;
int taskIdx = NUMTRAILS / 4 *0 +0;
int pIdx = 2;
// 0 study
// else training
int mode = 0;

MyArrayStr taskConfigs;
MyArrayi taskOrder;

void outputAnswer(){
	MyString decimer("\t");
	if(taskIdx%(taskOrder.size()/4) == 0){
		rstFile << std::endl;
	}
	MyString taskName = task->GetTaskName();
	taskName.resize(8);
	MyString layoutName = task->GetLayoutName();
	layoutName.resize(8);
	MyString visName = task->GetVisualElementName();
	visName.resize(8);
	rstFile << pIdx << decimer
		<< taskIdx << decimer
		<< taskOrder[taskIdx] << decimer
		<< taskName  << decimer
		<< layoutName  << decimer
		<< visName  << decimer ;
	if(task->GetTask()->GetNumOptions()>0){
		rstFile << task->GetTask()->GetAnswerIndex() << std::endl;
	}
	else{
		rstFile << task->GetTask()->GetAnswerValue() << std::endl;
	}
}

void NextTask(){
	SafeFreeObject(task);
	if(taskIdx == taskOrder.size()-1){
		rstFile.close();
		exit(1);
	}
	else{
		task = new MyGraphTaskInstance(taskIdx, NUMTRAILS);
		task->SetParticipantIndex(pIdx);
		task->LoadFromFile(taskConfigs[taskOrder[taskIdx]]);
		outputAnswer();
		taskIdx++;
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

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE | GLUT_ALPHA);
	glutInitWindowSize(50, 50);
	glutCreateWindow("Graph Explorer");
	
	rstFile.open("answer_"+MyString(pIdx)+".txt");

	if(mode == 0){

		MyString dropBoxFolder = "data\\task_configs_7\\";
		unsigned randSeed = 0;
		if(argc > 1){
			randSeed = atoi(argv[1]);
		}

		for (int i = 0; i<NUMTESTS; i++){
			taskConfigs << dropBoxFolder+MyString(i)+".txt";
		}
	
			/*
			L0D0 L0D1 L0D2
			L1D0 L1D1 L2D2
			L2D0 L2D1 L3D2
			*/
		int LatinSquare[3][3][2] ={
			{{0,0},{0,1},{0,2}},
			{{1,0},{1,1},{1,2}},
			{{2,0},{2,1},{2,2}}
		};
		int latinSeed = pIdx%3;
		for(int iTask = 0;iTask<4;iTask++){
			// things in the Latin Square
			int task_offSet = NUMTESTS/4 * iTask;
			for(int iLatin = 0;iLatin<3;iLatin++){
				int latin_x = (latinSeed+iLatin)%3;
				int latin_y = (latinSeed+iLatin)%3;
				int layoutIdx = LatinSquare[latin_x][latin_y][0];
				int diffIdx = LatinSquare[latin_x][latin_y][1];
				int diff_offSet = NUMTESTS/4/3 * diffIdx;
				int layout_offset = layoutIdx;
				MyArrayi permuteEncoding = MyArrayi::GetSequence(0,8);
				permuteEncoding.Permute(pIdx*iTask*iLatin);
				for(int iEncoding = 0;iEncoding < 9; iEncoding ++){
					int encoding_offset = permuteEncoding[iEncoding] * 3;
					taskOrder << task_offSet + diff_offSet + layout_offset + encoding_offset;
				}
			}
			// permute between task
			taskOrder.Permute(pIdx,NUMTRAILS/4*iTask, NUMTRAILS/4*(iTask+1)-1);
		}
	}
	else{

		NUMTRAILS = NUMTRAINING;
		MyString dropBoxFolder = "data\\task_configs_training_3\\";
		unsigned randSeed = 0;
		if(argc > 1){
			randSeed = atoi(argv[1]);
		}

		//taskConfigs<<dropBoxFolder+MyString("test")+".txt";
		for (int i = 0; i<NUMTRAINING; i++){
			taskConfigs << dropBoxFolder+MyString(i)+".txt";
		}
		taskOrder = MyArrayi::GetSequence(0,NUMTRAILS-1);
	}

	while(1){
		NextTask();
	};

	return 1;
}