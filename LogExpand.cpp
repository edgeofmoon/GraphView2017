#include "MyGraphTaskInstance.h"
#include "MyArray.h"
#include "MyUtility.h"
#include "VisConfig.h"
#include "MyDataLoader.h"
#include "MyGraphAlgorithm.h"
#include "MyGenericHierarchyOverlay.h"
#include <sstream>
#include <fstream>
using namespace std;
int TASKSET = 2;

typedef MySelectionEvent MyEvent;

#include <fstream>

std::ifstream rstFile;
std::ofstream outFile;
MyGraphTaskInstance* task = 0;

bool checkFile = false;

int taskIdx = 0;

int ccount = 0;

MyArrayStr taskConfigs;
MyArrayi taskOrder;

MyGenericEvent::MyMouseKey toKey(int k){
	return (k == 0 ?
		MyGenericEvent::MOUSE_KEY_LEFT : (k == 1 ?
		MyGenericEvent::MOUSE_KEY_MIDDLE : MyGenericEvent::MOUSE_KEY_RIGHT));
}

MyGenericEvent::MyKeyState toState(int s){
	return (s == 0 ? MyGenericEvent::KEY_DOWN : MyGenericEvent::KEY_UP);
}

bool emptyShown = false;

bool isSameLink(const MyVec2i& a, const MyVec2i& b){
	return (a == b)
		|| (a[1] == b[0] && a[0] == b[1]);
}

int getSide(int nodeIdx, MyArrayStr* labels) {
	MyString nodeName = labels->at(nodeIdx);
	if(nodeName.back()=='L' || nodeName.back()=='l'){
		return 0;
	}
	else{
		return 1;
	}
}

void NextTask(){
	int pid, trialIdx, difficulty, UserAns, CorrectAns, IsCorrect;
	float density, TimeUsed, logTime;
	MyString task, layout, encoding;
	rstFile >> pid >> trialIdx >> task >> layout >> encoding >> density
		>> difficulty >> UserAns >> CorrectAns >> TimeUsed >> logTime >> IsCorrect;

	MyString fn = taskConfigs[taskOrder[taskIdx]];

	ifstream infile(fn);
	if(!infile.is_open()){
		return;
	}
	MyString comment;
	MyString taskType;
	infile >> comment >> taskType;
	
	infile >> comment;
	MyString extraInfo;
	getline(infile, extraInfo, '\n');

	int m;
	MyArray3f* mPos;
	MyArrayStr* mLabels;
	MyGenericNode* mHierarchy;
	infile >> comment >> m;
	MyString hierarchyFn,posFn, labelFn;;
	infile >> comment >> hierarchyFn >> comment >> posFn >> comment >> labelFn;
	mHierarchy = MyDataLoader::LoadHierarchyOverlay(hierarchyFn);
	mPos =  MyDataLoader::MakeArrayVecFromFile(posFn, m);
	
	mLabels = MyDataLoader::MakeArrayStrFromFile(labelFn);

	int numGraph;
	float mDensity;
	infile >> comment >> numGraph;
	float maxValue = -999999, minValue=999999;
	MyArray<MyDenseGraphf*> graphs;
	for(int i = 0;i<numGraph;i++){
		MyString gfn;
		infile >> comment >> gfn >> comment >> mDensity;
		MyDenseGraphf* graph = MyDataLoader::MakeGraphFromMatrixFileFilterByEdgeRatio(gfn,m,m,mDensity);
		MyString layoutType, repType, bundlerType;
		int encoding;
		infile >> comment >> layoutType >> comment >> bundlerType
			>> comment >> encoding >> comment >> repType;
		graphs << graph;
	}
	
	if(task == "Task_Node_Variation"){
	//if(taskType == "task_degree_compare"){
		int nodeIdx = extraInfo.ToInt();
		MyGraphAlgorithm algor;
		algor.SetGraph(graphs[0]);
		MyArrayf values;
		MyArrayf value2;
		values = algor.GetDegreeCentrality();
		float tMaxValue1 = values[values.GetMaximumIndex()];
		value2 << values[nodeIdx];
		algor.SetGraph(graphs[1]);
		values = algor.GetDegreeCentrality();
		float tMaxValue2 = values[values.GetMaximumIndex()];
		value2 << values[nodeIdx];
		if(value2[0] > value2[1]){
			assert(CorrectAns == 0);
		}
		else{
			assert(CorrectAns == 1);
		}
		float maxValue = tMaxValue1>tMaxValue2? tMaxValue1:tMaxValue2;
		//outFile << value2[UserAns] << '\t' << value2[CorrectAns] << '\t' << maxValue << '\t' << ccount << endl;
		outFile << value2[0] << '\t' << value2[1] << '\t' << "0\t0\t" << maxValue << '\t' << ccount << endl;
		//cout << value2[UserAns] << '\t' << value2[CorrectAns] << '\t' << maxValue << endl;
	}
	else if(task == "Task_variation"){
	//else if(taskType == "task_variation"){
		stringstream ss(extraInfo);
		int two, mClusterIndices[2];
		ss >> two >> mClusterIndices[0] >> mClusterIndices[1];
		MyArrayi node2lobe(74, -1);
		for (int i = 0; i<mHierarchy->GetNumChildren(); i++){
			const MyGenericNode* child = mHierarchy->GetChild(i);
			MyArrayStr nodesStr;
			MyGenericHierarchyOverlayStr::AddLeaveObjectToArray(&nodesStr, child);
			for (int j = 0; j < nodesStr.size(); j++){
				int node = nodesStr[j].ToInt();
				node2lobe[node] = i;
			}
		}

		for (int i = 0; i < node2lobe.size(); i++){
			assert(node2lobe[i] >= 0);
		}

	
		// this is purely link count change
		// not number of links change
		MyArrayi lobeNumEdge0(mHierarchy->GetNumChildren(), 0);
		MyArrayi lobeNumEdge1(mHierarchy->GetNumChildren(), 0);

		for (int i = 0; i < graphs[0]->GetNumEdges(); i++){
			MyVec2i edge = graphs[0]->GetEdge(i);
			int lobe0 = node2lobe[edge[0]];
			int lobe1 = node2lobe[edge[1]];
			if (lobe0 == lobe1){
				lobeNumEdge0[lobe0] ++;
			}
			else{
				lobeNumEdge0[lobe0] ++;
				lobeNumEdge0[lobe1] ++;
			}
		}

		for (int i = 0; i < graphs[1]->GetNumEdges(); i++){
			MyVec2i edge = graphs[1]->GetEdge(i);
			int lobe0 = node2lobe[edge[0]];
			int lobe1 = node2lobe[edge[1]];
			if (lobe0 == lobe1){
				lobeNumEdge1[lobe0] ++;
			}
			else{
				lobeNumEdge1[lobe0] ++;
				lobeNumEdge1[lobe1] ++;
			}
		}

		MyArrayi variations;
		MyArray2i linkNumbers;
		for (int i = 0; i < mHierarchy->GetNumChildren(); i++){
			int change = lobeNumEdge0[i] - lobeNumEdge1[i];
			linkNumbers << MyVec2i(lobeNumEdge0[i],lobeNumEdge1[i]);
			if(change <0) change = -change;
			variations << change;
		}
		int mAnswerIndex;
		if(variations[mClusterIndices[0]] > variations[mClusterIndices[1]]){
			mAnswerIndex = mClusterIndices[0];
			assert(CorrectAns == mAnswerIndex);
		}
		else{
			 mAnswerIndex = mClusterIndices[1];
			assert(CorrectAns == mAnswerIndex);
		}
		outFile << linkNumbers[mClusterIndices[0]][0] << '\t' << linkNumbers[mClusterIndices[0]][1]
			<< '\t' << linkNumbers[mClusterIndices[1]][0] << '\t' << linkNumbers[mClusterIndices[1]][1]
			<< '\t' << graphs[0]->GetNumEdges() << '\t' << ccount << endl;
		//cout << variations[mAnswerIndex] << '\t' << variations[UserAns] << '\t' << -1<< endl;
	}
	else if(task == "Task_Hub_Node"){
	//else if(taskType == "task_hub"){
		MyGraphAlgorithm algor;
		algor.SetGraph(graphs[0]);
		MyArrayf values;
		MyArrayf value2;
		algor.FloydWarshallWithPathReconstruction();
		values = algor.GetBetweennessCentrality();
		float maxValue1 = values[values.GetMaximumIndex()];
		outFile << values[CorrectAns] << '\t';
		outFile << values[UserAns] << '\t' << "0\t0\t" << maxValue1 << '\t' << ccount << endl;
		//cout << values[CorrectAns] << '\t';
		//cout << values[UserAns] << '\t' << maxValue1 << endl;
	}
	else if (task == "Task_Inter_Lobe"){
	//else if (taskType == "task_inter_lobe"){
		stringstream ss(extraInfo);
		int two, mClusterIndices[2];
		ss >> two >> mClusterIndices[0] >> mClusterIndices[1];
		MyArrayi node2cluster(graphs[0]->GetNumNodes(),-1);
		for(int i = 0;i<mHierarchy->GetNumChildren();i++){
			MyArrayStr nodeIdxStrs;
			MyGenericHierarchyOverlayStr::AddLeaveObjectToArray(&nodeIdxStrs,mHierarchy->GetChild(i));
			for(int inode = 0;inode<nodeIdxStrs.size();inode++){
				int nodeIdx = nodeIdxStrs[inode].ToInt();
				node2cluster[nodeIdx] = i;
			}
		}
		for (int i = 0; i < node2cluster.size(); i++){
			assert(node2cluster[i] >= 0);
		}

		MyArrayi numLinks(2,0);
		for(int igraph = 0;igraph <2;igraph++){
			const MyGenericGraphf* graph = graphs[igraph];
			for(int iedge = 0;iedge <graph->GetNumEdges();iedge++){
				MyVec2i edge = graph->GetEdge(iedge);
				MyVec2i edgeCluster(node2cluster[edge[0]],node2cluster[edge[1]]);
				if(isSameLink(edgeCluster, mClusterIndices)){
					numLinks[igraph]++;
				}
			}
		}
	
		int mAnswerIndex;
		if(numLinks[0] < numLinks[1]){
			mAnswerIndex = 1;
		}
		else if(numLinks[0] > numLinks[1]){
			mAnswerIndex = 0;
		}
		else{
			// cannot be same
			mAnswerIndex = -1;
		}
		assert(CorrectAns == mAnswerIndex);
		outFile << numLinks[0] << '\t' << numLinks[1] 
			<< '\t' << "0\t0\t" << graphs[0]->GetNumEdges() << '\t' << ccount << endl;
		//cout << numLinks[mAnswerIndex] << '\t' << numLinks[UserAns] << '\t' << -1 << endl;
	}
	else if (task == "Task_Lobe_Hub"){
	//else if (taskType == "task_lobe_hub"){
		int mClusterIndex = extraInfo.ToInt();
		const MyGenericGraphf* graph = graphs[0];
		MyGraphAlgorithm algor;
		algor.SetGraph(graph);
		algor.FloydWarshallWithPathReconstruction();
		MyArrayf betweennessCentrality = algor.GetBetweennessCentrality();

		const MyGenericNode* cluster = mHierarchy->GetChild(mClusterIndex);
		MyArrayStr idxStrs;
		MyGenericHierarchyOverlayStr::AddLeaveObjectToArray(&idxStrs,cluster);

		MyArrayf btns;
		for(int i = 0;i<idxStrs.size();i++){
			int idx = idxStrs[i].ToInt();
			float btc = betweennessCentrality[idx];
			btns << btc;
		}

		MyArrayi* sort = btns.MakeSortResultArray();
		int mAnswerIndex = idxStrs[sort->back()].ToInt();
		assert(CorrectAns == mAnswerIndex);
		float maxValue1 = betweennessCentrality[betweennessCentrality.GetMaximumIndex()];
		outFile << betweennessCentrality[mAnswerIndex] << '\t' << betweennessCentrality[UserAns] << 
			'\t' << "0\t0\t" << maxValue1 << '\t' << ccount << endl;
		//cout << betweennessCentrality[mAnswerIndex] << '\t' << betweennessCentrality[UserAns] << 
		//	'\t' << maxValue1 << endl;
		delete sort;
	}
	else if (task == "Task_Opposite_Hub"){
	//else if (taskType == "task_opposite_hub"){
		int mNodeIdx = extraInfo.ToInt();
		int side = getSide(mNodeIdx, mLabels);

		const MyGenericGraphf* graph = graphs[0];
		MyArrayi neighbors = graph->GetNeighbors(mNodeIdx);
	
		MyArrayi oppositeNeighbors;
		for(int i = 0;i<neighbors.size();i++){
			if(getSide(neighbors[i], mLabels) != side){
				oppositeNeighbors << neighbors[i];
			}
		}
		MyGraphAlgorithm algor;
		algor.SetGraph(graph);
		algor.FloydWarshallWithPathReconstruction();
		MyArrayf betweennessCentrality = algor.GetBetweennessCentrality();

		int hubIdx = neighbors[0];
		float mBt = -1;
		for(int i = 0;i<oppositeNeighbors.size();i++){
			int nodeIdx = oppositeNeighbors[i];
			float bt = betweennessCentrality[nodeIdx];
			if(bt>mBt){
				mBt = bt;
				hubIdx = nodeIdx;
			}
		}

		int mAnswerIndex = hubIdx;
		assert(CorrectAns == mAnswerIndex);
		float maxValue1 = betweennessCentrality[betweennessCentrality.GetMaximumIndex()];
		outFile << betweennessCentrality[mAnswerIndex] << '\t' << betweennessCentrality[UserAns] << 
			'\t' << "0\t0\t" << maxValue1 << '\t' << ccount << endl;
		//cout << betweennessCentrality[mAnswerIndex] << '\t' << betweennessCentrality[UserAns] << 
		//	'\t' << maxValue1 << endl;
	}
	else {
		assert(0);
	}
	
	delete mPos;
	delete mLabels;
	delete mHierarchy;
	for(int i = 0;i<numGraph;i++){
		delete graphs[i];
	}
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
	int pIdx = 3;

	// 0 study
	// else training
	int mode = 0;
	pIdx = 3;
	
	rstFile.open("C:\\Users\\GuohaoZhang\\Dropbox\\SAS\\connection\\results_itr5.txt");
	outFile.open("C:\\Users\\GuohaoZhang\\Dropbox\\SAS\\connection\\results_itr5_appendix.txt");
	
	MyString header;
	getline(rstFile, header, '\n');
	/*
	for(TASKSET = 1; TASKSET <=2; TASKSET ++){
		int maxTest = 54*4*3;
		if(TASKSET == 2) maxTest = 54*2*3;
		MyString dropBoxFolder;
		if(TASKSET == 1){
			dropBoxFolder = "data\\task_configs_74_dualDensity_2\\";
		}
		else{
			dropBoxFolder = "data\\task_configs_74_dualDensity_3\\";
		}
		unsigned randSeed = pIdx;

		for (int i = 0; i<maxTest; i++){
			taskConfigs << dropBoxFolder+MyString(i)+".txt";
		}
		taskOrder = MyArrayi::GetSequence(0,maxTest-1);
		for(int i = 0;i<maxTest;i++){
			NextTask();
			taskIdx = i;
		}
	}
	return 1;
	*/

	for(pIdx = 3; pIdx <= 21; pIdx ++){
		for (int ittt = 0; ittt < 6*324+216+26; ittt++)
		{
			//getline(rstFile, header, '\n');

		}
		if (pIdx == 9){
			continue;
		}
		for(TASKSET = 1; TASKSET <=2; TASKSET ++){
		
			cout << pIdx << '\t' << TASKSET << endl;

			taskIdx = 0;
			int NUMTESTS = 648;
			int NUMTASKS = 4;
			int NUMTRAINING = 36;

			if(TASKSET == 1){
			}
			else{
				NUMTESTS = 324;
				NUMTASKS = 2;
				NUMTRAINING = 18;
			}
			int NUMTRAILS = NUMTESTS/3;

			MyString dropBoxFolder;
			if(TASKSET == 1){
				dropBoxFolder = "data\\task_configs_74_dualDensity_2\\";
			}
			else{
				dropBoxFolder = "data\\task_configs_74_dualDensity_3\\";
			}
			unsigned randSeed = pIdx;

			taskConfigs.clear();
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
			int block_task = NUMTESTS/NUMTASKS;
			int block_difficulty = 54;
			int block_encoding = 6;
			int block_layout = 2;
			int block_density = 1;

			taskOrder.clear();
			for(int iTask = 0;iTask<NUMTASKS;iTask++){
				// things in the Latin Square
				int task_offSet = block_task * iTask;
				for(int iLatin = 0;iLatin<3;iLatin++){
					int latin_x = (latinSeed+iLatin)%3;
					int latin_y = (latinSeed+iLatin)%3;
					int layoutIdx = LatinSquare[latin_x][latin_y][0];
					int diffIdx = LatinSquare[latin_x][latin_y][1];
					int diff_offSet = block_difficulty * diffIdx;
					int layout_offset = block_layout*layoutIdx;
					MyArrayi permuteEncoding = MyArrayi::GetSequence(0,8);
					permuteEncoding.Permute(pIdx*iTask*iLatin);
					for(int iEncoding = 0;iEncoding < 9; iEncoding ++){
						int encoding_offset = permuteEncoding[iEncoding] * block_encoding;
						for(int iDensity = 0;iDensity <2; iDensity ++){
							int density_offset = iDensity*block_density;
							taskOrder << task_offSet + diff_offSet + layout_offset + encoding_offset + density_offset;
						}
					}
				}
				// permute between task
				taskOrder.Permute(pIdx,block_task/3*iTask, block_task/3*(iTask+1)-1);
			}
			assert(taskOrder.size () == NUMTRAILS);
			for(int ito = 0;ito<taskOrder.size();ito++){
				taskIdx = ito;
				//if (TASKSET == 1) break;
				NextTask();
				ccount++;
				//cout << pIdx << " " << TASKSET << " " << ito << endl;
			}
		}
	}


	return 1;
}