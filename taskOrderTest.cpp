#include "MyArray.h"
#include "MyVisualElement.h"

#include <fstream>
#include <iostream>
#include <cassert>
using namespace std;

std::ofstream rstFile;

int NUMTESTS = 648;
int NUMTASKS = 4;
int NUMTRAINING = 36;
int taskIdx = 0;
int pIdx = 2;
// 0 study
// else training
int mode = 0;

MyArrayStr taskConfigs;
MyArrayi taskOrder;

void configToRow(MyString fn){
	ifstream infile(fn);
	if (!infile.is_open()){
		return;
	}
	MyString comment, taskType;
	MyString layoutType, repType, bundlerType;
	MyString hierarchyFn, posFn, labelFn;
	int numGraph, difficulty, encoding, background;
	float density;

	infile >> comment >> taskType;
	infile >> comment;

	string line;
	getline(infile, line, '\n');

	int m;
	infile >> comment >> m;
	infile >> comment >> hierarchyFn >> comment >> posFn >> comment >> labelFn;
	infile >> comment >> numGraph;

	for (int i = 0; i < numGraph; i++){
		MyString gfn;
		infile >> comment >> gfn >> comment >> density;
		infile >> comment >> layoutType >> comment >> bundlerType
			>> comment >> encoding >> comment >> repType;
	}
	infile >> comment >> difficulty;
	infile >> comment >> background;
	infile.close();


	MyString encodingName = MyVis::GetVisualElementName((MyVis::MyVisualElement)encoding);
	MyString decimer("\t");
	rstFile << pIdx << decimer
		<< taskIdx << decimer
		<< taskType << decimer
		<< layoutType << decimer
		<< encodingName << decimer
		<< difficulty << decimer
		<< density << decimer
		<< (background == 0 ? "white" : "gray") << endl;
}

int main(int argc, char* argv[]){

	mode = 1;
	rstFile.open("orderTest.txt");
	MyString decimer("\t");
	rstFile << "pIdx" << decimer
		<< "taskIdx" << decimer
		<< "taskType" << decimer
		<< "layoutType" << decimer
		<< "encodingName" << decimer
		<< "difficulty" << decimer
		<< "density" << decimer
		<< "background" << endl;

	if (mode == 0){
		MyString dropBoxFolder = "data\\task_configs_74_dualDensity_dualbackground\\";
		int LatinSquare[3][3][2] = {
			{ { 0, 0 }, { 1, 1 }, { 2, 2 } },
			{ { 1, 0 }, { 2, 1 }, { 0, 2 } },
			{ { 2, 0 }, { 0, 1 }, { 1, 2 } }
		};
		int block_task = NUMTESTS / NUMTASKS;
		int block_difficulty = 54;
		int block_encoding = 6;
		int block_layout = 2;
		int block_density = 1;
		for (int i = 0; i<NUMTESTS; i++){
			taskConfigs << dropBoxFolder + MyString(i) + ".txt";
		}
		for (int ip = 0; ip < 6; ip++){
			pIdx = ip;
			// background replaces density
			int block_background = block_density;
			for (int iTask = 0; iTask < NUMTASKS; iTask++){
				// things in the Latin Square
				int task_offSet = block_task * iTask;
				for (int iBg = 0; iBg < 2; iBg++){
					int orderSize = taskOrder.size();
					int background_offset = block_background * (pIdx % 2 == 0 ? iBg : 1 - iBg);
					for (int iLatin = 0; iLatin < 3; iLatin++){
						int latin_x = (pIdx + iLatin) % 3;
						int latin_y = (pIdx / 2) % 3;
						int layoutIdx = LatinSquare[latin_y][latin_x][0];
						int diffIdx = LatinSquare[latin_y][latin_x][1];
						int diff_offSet = block_difficulty * diffIdx;
						int layout_offset = block_layout*layoutIdx;
						MyArrayi permuteEncoding = MyArrayi::GetSequence(0, 8);
						permuteEncoding.Permute(pIdx*iTask*iLatin);
						for (int iEncoding = 0; iEncoding < 9; iEncoding++){
							int encoding_offset = permuteEncoding[iEncoding] * block_encoding;
							taskOrder << task_offSet + background_offset + diff_offSet + layout_offset + encoding_offset;
						}
					}
					// permute between background
					taskOrder.Permute(pIdx, orderSize, taskOrder.size() - 1);
				}
			}
			assert(taskOrder.size() == NUMTESTS / 3);

			for (int i = 0; i < taskOrder.size(); i++){
				cout << pIdx << ":" << i << "/" << taskOrder.size() - 1 << "\r         ";
				taskIdx = i;
				configToRow(taskConfigs[taskOrder[i]]);
			}
			taskOrder.clear();
		}
	}
	else{
		MyString dropBoxFolder = "data\\task_configs_74_dualDensity_dualbackground_training\\";

		//taskConfigs<<dropBoxFolder+MyString("test")+".txt";
		for (int i = 0; i<NUMTRAINING; i++){
			taskConfigs << dropBoxFolder + MyString(i) + ".txt";
		}
		taskOrder = MyArrayi::GetSequence(0, NUMTRAINING - 1);
		for (int i = 0; i < taskOrder.size(); i++){
			cout << pIdx << ":" << i << "/" << taskOrder.size() - 1 << "\r         ";
			taskIdx = i;
			configToRow(taskConfigs[taskOrder[i]]);
		}
		taskOrder.clear();
	}

	return 1;
}