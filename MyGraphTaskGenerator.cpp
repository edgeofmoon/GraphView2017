#include "MyGraphTaskGenerator.h"
#include "MyDataLoader.h"
#include "MyUtility.h"
#include "MyGraphAlgorithm.h"
#include "MyGenericHierarchyOverlay.h"
#include "VisConfig.h"
#include <iostream>
#include <fstream>
#include <cassert>
using namespace std;

#define DEGREE_AVARAGE 6
#define DEGREE_DIFFERENCE 0.3
#define NEIGHBOR_AVERAGE 2
#define HUBDEGREE_AVERAGE 4

#define MINIMALDIFF 0.05

MyGraphTaskGenerator::MyGraphTaskGenerator(void)
{
	//mFolder = "configs\\";
	//mFolder = "data\\task_configs_74_dualDensity_2\\";
	//mFolder = "data\\task_configs_74_dualDensity_dualbackground\\";
	mFolder = "data\\task_configs_74_dualDensity_dualbackground_training\\";
	mDecimer = "\t";
}


MyGraphTaskGenerator::~MyGraphTaskGenerator(void)
{
}

// if task gen is unsuccessful
// healthy is set to false
bool healthy = true;

void MyGraphTaskGenerator::GenerateConfigureFiles(){
	MyArray<MyVis::MyVisualElement> encodings;
	encodings	<< MyVis::LENGTH
				<< MyVis::SIZE
				<< MyVis::ANGLE
				<< MyVis::SATURATION
				<< MyVis::HUE
				<< MyVis::TEXTURE
				<< MyVis::DENSITY
				<< MyVis::SHAPE
				<< MyVis::SLOPE;
	MyArray<VisSpec::Geometry> vis;
	vis << VisSpec::GEO_RAW 
		<< VisSpec::GEO_TREERING
		<< VisSpec::GEO_MATRIX;
	MyArray<VisSpec::Density> density;
	density << VisSpec::SPARSE
		<< VisSpec::MEDIUM;
	MyArray<VisSpec::Background> background;
	background << VisSpec::WHITE
		<< VisSpec::GRAY;

	MyArray<VisSpec> specs;
	for(int i = 0;i<encodings.size();i++){
		for(int j = 0;j<vis.size();j++){
			/*
			for(int k = 0;k<density.size();k++){
				specs << VisSpec(density[k],vis[j],VisSpec::TOPO_BUNDLED,encodings[i]);
			}
			*/
			for (int k = 0; k < background.size(); k++){
				specs << VisSpec(VisSpec::SPARSE, vis[j], VisSpec::TOPO_BUNDLED, encodings[i], background[k]);
			}
		}
	}

	MyArrayStr tasks;
	tasks	//<< "task_degree"
			<< "task_degree_compare"
			//<< "task_neighbor"
			<< "task_hub"
			//<< "task_inter_lobe"
			//<< "task_variation";
			<< "task_lobe_hub"
			//<< "task_active_lobe"
			<< "task_opposite_hub";


	MyArrayi regions;
	//regions << 0 << 1 << 2 << 3;
	regions << -1;

	/*
	int repeat = 1;
	int idx = 0;
	for(int itask = 0;itask<tasks.size();itask++){
		MyString task = tasks[itask];
		for (int ireg = 0; ireg < regions.size(); ireg++){
			int regIdx = regions[ireg];
			for(int ispec = 0;ispec<specs.size();ispec++){
				VisSpec spec = specs[ispec];
				for (int irep = 0; irep<repeat; irep++){
					MyString filename(mFolder + MyString(idx) + ".txt");
					ofstream configFile;
					//if(task != "task_degree_compare"){
					//	idx++;
					//	continue;
					//}
					do{
						configFile.open(filename);
						if (!configFile.is_open()){
							cout << "cant open file " << filename << endl;
							exit(0);
						}
						healthy = true;
						int idx1 = rand() % mMatrixFileNames.size();
						int idx2 = rand() % mMatrixFileNames.size();
						while (idx2 == idx1){
							idx2 = rand() % mMatrixFileNames.size();
						}
						MyArrayi idxs;
						idxs << idx1 << idx2;
						MyArrayi indices = this->ComputeExtraInfo(task, spec, mMatrixFileNames[idxs[0]], mMatrixFileNames[idxs[1]], regIdx);
						WriteConfigure(configFile, task, spec, idxs, indices, regIdx);
						configFile.close();
					}while(!healthy);
					idx++;
				}
			}
		}
	}
	*/
	int mNumNodes[] = {74,74};
	float mDensity[] = {0.05, 0.10};
	if(DATASET == 0){
		/*
		// 74 dataset
		mMeanGraphDegreeSegs.clear();
		// avoid zero degree?
		mMeanGraphDegreeSegs << MyVec2f(0.0001,0.0137)
							<< MyVec2f(0.0137, 0.0684932)
							<< MyVec2f(0.0684932, 1);
		mDegreeDiffSegs.clear();
		mDegreeDiffSegs	<< MyVec2f(-2,-0.0273973)
						<< MyVec2f(-0.0273973, 0.0136986)
						<< MyVec2f(0.0136986, 2);
		mCommonNeighborSegs.clear();
		mCommonNeighborSegs	<< MyVec2i(1,1)
							<< MyVec2i(2,2)
							<< MyVec2i(3,3);
		mNeighborRange	<< MyVec2i(2,2)
						<< MyVec2i(3,3)
						<< MyVec2i(4,4);
		mLinkChangeDiffRange << MyVec2i(20,9999)
						 << MyVec2i(20,9999)
						  << MyVec2i(20,9999);
		mOppositeNeighborRange	<< MyVec2i(2,3)
							<< MyVec2i(4,5)
							<< MyVec2i(5,7);
		mInterLobeLinkDiffRange << MyVec2i(1,2)
								<< MyVec2i(2,4)
								<< MyVec2i(5,11);
		mMeanGraphHubNodeThres = 0.03;
		*/
		// difficult version
		mDegreeDiffSegs	<< MyVec2f(0.5/mNumNodes[0], 1.5/mNumNodes[0])
						<< MyVec2f(1.5/mNumNodes[0], 2.5/mNumNodes[0])
						<< MyVec2f(2.5/mNumNodes[0], 3.5/mNumNodes[0])
						<< MyVec2f(0.5/mNumNodes[0], 1.5/mNumNodes[0])
						<< MyVec2f(1.5/mNumNodes[0], 2.5/mNumNodes[0])
						<< MyVec2f(2.5/mNumNodes[0], 3.5/mNumNodes[0]);
		/*
		// simple version
		mDegreeDiffSegs	<< MyVec2f(3.552755159/mNumNodes[0], 5.597482771/mNumNodes[0])
						<< MyVec2f(5.597482771/mNumNodes[0], 7.642210382/mNumNodes[0])
						<< MyVec2f(7.642210382/mNumNodes[0], 15.f/mNumNodes[0])
						<< MyVec2f(5.794058343/mNumNodes[1], 9.059083148/mNumNodes[1])
						<< MyVec2f(9.059083148/mNumNodes[1], 12.32410795/mNumNodes[1])
						<< MyVec2f(12.32410795/mNumNodes[1], 22.f/mNumNodes[1]);
		*/
		mLinkChangeDiffRange<< MyVec2i(9.033824072, 14.14248857)
							<< MyVec2i(14.14248857,19.25115308)
							<< MyVec2i(19.25115308,34)
							<< MyVec2i(14.53540942, 22.58852551)
							<< MyVec2i(22.58852551,30.6416416)
							<< MyVec2i(30.6416416,43);
		mMeanGraphHubNodeThres	<< 0.047114978 
								<< 0.047114978 
								<< 0.047114978 
								<< 0.028987518
								<< 0.028987518
								<< 0.028987518;
		mInterLobeLinkDiffRange << MyVec2i(3.74021166,5.930127276)
								<< MyVec2i(5.930127276,8.120042892)
								<< MyVec2i(8.120042892,17)
								<< MyVec2i(6.48051279,10.208821)
								<< MyVec2i(10.208821,13.9371292)
								<< MyVec2i(13.9371292,23);
		mOppositeNeighborRange	<< MyVec2i(2,3)
								<< MyVec2i(4,5)
								<< MyVec2i(5,7)
								<< MyVec2i(4,5)
								<< MyVec2i(6,7)
								<< MyVec2i(8,10);
		mNeighborRange	<< MyVec2i(2,2)
						<< MyVec2i(3,3)
						<< MyVec2i(4,4)
						<< MyVec2i(4,4)
						<< MyVec2i(5,5)
						<< MyVec2i(6,6);
	}
	else if(DATASET == 1){
		/*
		// 167 dataset
		mMeanGraphDegreeSegs.clear();
		// avoid zero degree?
		mMeanGraphDegreeSegs << MyVec2f(0.0001,0.00903615)
							<< MyVec2f(0.00903615, 0.0843373)
							<< MyVec2f(0.0843373, 1);
		mDegreeDiffSegs.clear();
		mDegreeDiffSegs	<< MyVec2f(-2,-0.0240964)
						<< MyVec2f(-0.0240964, 0.0180723)
						<< MyVec2f(0.0180723, 2);
		mCommonNeighborSegs.clear();
		mCommonNeighborSegs	<< MyVec2i(1,1)
							<< MyVec2i(2,2)
							<< MyVec2i(3,3);
		mNeighborRange	<< MyVec2i(2,2)
						<< MyVec2i(3,3)
						<< MyVec2i(4,4);
		mLinkChangeRange << MyVec2i(15,35)
						 << MyVec2i(35,55)
						  << MyVec2i(55,9999);
		mLinkChangeDiffRange << MyVec2i(85,9999)
						 << MyVec2i(85,9999)
						  << MyVec2i(85,9999);
		// need to test the 2
		mOppositeNeighborRange	<< MyVec2i(5,7)
							<< MyVec2i(8,10)
							<< MyVec2i(11,15);
		mInterLobeLinkDiffRange << MyVec2i(6,9)
								<< MyVec2i(10,15)
								<< MyVec2i(16,50);
		mMeanGraphHubNodeThres = 0.01;
		*/
		//100 dataset
		mMeanGraphDegreeSegs.clear();
		// avoid zero degree?
		mMeanGraphDegreeSegs << MyVec2f(0.0001,0.0303615)
							<< MyVec2f(0.0303615, 0.0443373)
							<< MyVec2f(0.0443373, 1);
		mDegreeDiffSegs.clear();
		mDegreeDiffSegs	<< MyVec2f(-2,-0.0240964)
						<< MyVec2f(-0.0240964, 0.0180723)
						<< MyVec2f(0.0180723, 2);
		mCommonNeighborSegs.clear();
		mCommonNeighborSegs	<< MyVec2i(1,1)
							<< MyVec2i(2,2)
							<< MyVec2i(3,3);
		mNeighborRange	<< MyVec2i(2,2)
						<< MyVec2i(3,3)
						<< MyVec2i(4,4);
		mLinkChangeRange << MyVec2i(15,35)
						 << MyVec2i(35,55)
						  << MyVec2i(55,9999);
		mLinkChangeDiffRange << MyVec2i(85,9999)
						 << MyVec2i(85,9999)
						  << MyVec2i(85,9999);
		// need to test the 2
		mOppositeNeighborRange	<< MyVec2i(3,5)
							<< MyVec2i(5,8)
							<< MyVec2i(8,15);
		mInterLobeLinkDiffRange << MyVec2i(5,7)
								<< MyVec2i(8,12)
								<< MyVec2i(13,50);
		mMeanGraphHubNodeThres << 0.02;
	}

	/*
	// study
	MyArrayi difficuties;
	difficuties << 0 << 1 << 2;
	int repeat = 1;
	for(int itask = 0;itask<tasks.size();itask++){
		MyString task = tasks[itask];
		for (int idiff = 0; idiff < difficuties.size(); idiff++){
			int diffIdx = difficuties[idiff];
			for(int ispec = 0;ispec<specs.size();ispec++){
				VisSpec spec = specs[ispec];
				for (int irep = 0; irep<repeat; irep++){
					int idx = irep 
						+ispec*repeat+
						idiff*specs.size()*repeat+
						itask*difficuties.size()*specs.size()*repeat;
					MyString filename(mFolder + MyString(idx) + ".txt");
					ofstream configFile;
					do{
						configFile.open(filename);
						if (!configFile.is_open()){
							cout << "can't open file " << filename << endl;
							exit(0);
						}
						mHealthy = true;
						int idx = rand() % mMatrixFileNames.size();
						MyArrayi indices;
						if(task == "task_opposite_hub"
							|| task == "task_hub"
							|| task == "task_lobe_hub"){
							indices = this->ComputeExtraInfoAtDifficulty(task, spec, mMatrixFileNames[idx],  mMeanMatrixFileNames,diffIdx, -1);
						}
						else {
							indices = this->ComputeExtraInfoAtDifficulty(task, spec, mMeanMatrixFileNames, mMatrixFileNames[idx], diffIdx, -1);
						}

						MyArrayStr matFiles;
						matFiles << mMeanMatrixFileNames << mMatrixFileNames[idx];
						
						if(task == "task_active_lobe"
							|| task == "task_opposite_hub"
							|| task == "task_hub"
							|| task == "task_lobe_hub"){
							matFiles.Swap(0,1);
						}
						WriteConfigure(configFile, task, spec, matFiles, indices, difficuties[idiff]);
						configFile.close();
					}while(!mHealthy);
				}
			}
		}
	}
	
	*/
	
	// training
	int idx = 0;
	int repeat = 1;
	for(int itask = 0;itask<tasks.size();itask++){
		MyString task = tasks[itask];
		for(int iEnc = 0;iEnc<9;iEnc++){
			MyVis::MyVisualElement enc = encodings[iEnc];
			VisSpec::Geometry layout = vis[(itask*9+iEnc) % 3];
			int iBackGround = (itask % 2 == 0) ^ (iEnc < 5);
			MyString filename(mFolder + MyString(idx) + ".txt");
			ofstream configFile;
			do{
				configFile.open(filename);
				if (!configFile.is_open()){
					cout << "can't open file " << filename << endl;
					exit(0);
				}
				int difficulty = 1;
				if(task == "task_degree_compare"
					|| task == "task_inter_lobe"){
					difficulty = rand()%3;
				}
				mHealthy = true;
				int idx = rand() % mMatrixFileNames.size();
				MyArrayi indices;
				VisSpec spec(VisSpec::SPARSE, layout, VisSpec::TOPO_BUNDLED, enc, background[iBackGround]);
				if(task == "task_opposite_hub"
					|| task == "task_hub"
					|| task == "task_lobe_hub"){
					indices = this->ComputeExtraInfoAtDifficulty(task, spec, mMatrixFileNames[idx],  mMeanMatrixFileNames,difficulty, -1);
				}
				else {
					indices = this->ComputeExtraInfoAtDifficulty(task, spec, mMeanMatrixFileNames, mMatrixFileNames[idx], difficulty, -1);
				}

				MyArrayStr matFiles;
				matFiles << mMeanMatrixFileNames << mMatrixFileNames[idx];

				if(task == "task_active_lobe"
					|| task == "task_opposite_hub"
					|| task == "task_hub"
					|| task == "task_lobe_hub"){
					matFiles.Swap(0,1);
				}
				
				WriteConfigure(configFile, task, spec, matFiles, indices, difficulty);
				configFile.close();
			}while(!mHealthy);
			idx ++;
		}
	}
}

void MyGraphTaskGenerator::WriteConfigure(std::ostream& out, const MyString& task, const VisSpec& spec, MyArrayStr matFiles, const MyArrayi& indices, int difficulty) const{
	out << "[task_name]" << mDecimer << task << endl;

	//out << "[encoding]" << mDecimer <<

	out << "[extr_info]";
	if(indices.size() > 1){
		out << mDecimer << indices.size();
	}
	for(int i = 0;i<indices.size();i++){
		out << mDecimer << indices[i];
	}
	out << endl;
	
	out << "[num_nodes]" << mDecimer << NUMNODE << endl;

	out << "[hier_file]" << mDecimer << mHierarchyFileName << endl
		<< "[pos_file]" << mDecimer << mPositionFile << endl
		<< "[label_file]" << mDecimer << mLabelFile << endl;

	int numGraph = 1;
	if(task == "task_degree_compare"
		|| task == "task_variation"
		|| task == "task_inter_lobe") {
		numGraph = 2;
	}
	out << "[num_graph]" << mDecimer << numGraph << endl;
	for(int i = 0;i<numGraph;i++){
		out << "[matrix_file]" << mDecimer << matFiles[i] << endl
			<< "[density]" << mDecimer << spec.GetDensityValue() << endl
			<< "[geometry]" << mDecimer << spec.GetGeometryValue() << endl
			<< "[topology]" << mDecimer << spec.GetTopologyValue() << endl
			<< "[encoding]" << mDecimer << spec.GetVisualElement() << endl
			<< "[represent]" << mDecimer << this->GetRepresentation(spec) << endl;
	}
	out << "[difficulty]" << mDecimer << difficulty << endl;
	out << "[background]" << mDecimer << spec.GetBackground() << endl;
}

MyString MyGraphTaskGenerator::GetRepresentation(const VisSpec& spec) const{
	switch (spec.GetGeometry())
	{
	case VisSpec::GEO_MATRIX:
		return "representation_matrix";
	case VisSpec::GEO_RAW:
	case VisSpec::GEO_TREERING:
	default:
		return "representation_default";
	}
}

MyArrayi MyGraphTaskGenerator::ComputeExtraInfo(const MyString& task, const VisSpec& spec, const MyString& matfile, const MyString& matfile2, int regionIndex) const{
	MyGenericGraphf* graph = MyDataLoader::MakeGraphFromMatrixFileFilterByEdgeRatio(matfile,NUMNODE,NUMNODE,spec.GetDensityValue());
	MyArrayi rst;
	if(task == "task_degree"){
		return MyArrayi(1,this->GetNodeOfAboutDegree(DEGREE_AVARAGE, graph, regionIndex));
	}
	else if(task == "task_degree_compare"){
		MyGenericGraphf* graph2 = MyDataLoader::MakeGraphFromMatrixFileFilterByEdgeRatio(matfile2,NUMNODE,NUMNODE,spec.GetDensityValue());
		rst = MyArrayi(1,this->GetNodePairAboutDegreeDifference(DEGREE_DIFFERENCE, graph, graph2, regionIndex));
		delete graph2;
	}
	else if(task == "task_neighbor"){
		//MyGenericGraphf* graph2 = MyDataLoader::MakeGraphFromMatrixFileFilterByEdgeRatio(matfile2,NUMNODE,NUMNODE,spec.GetDensityValue());
		MyVec2i nodes = this->GetNodePairAboutNeighbor(NEIGHBOR_AVERAGE, graph, regionIndex);
		rst << nodes[0] << nodes[1];
	}
	else{
		assert(task == "task_hub");
		rst = MyArrayi(1, this->GetNodeOfAboutDegree(HUBDEGREE_AVERAGE, graph, regionIndex));
	}
	delete graph;
	return rst;
}

int MyGraphTaskGenerator::GetNodeOfAboutDegree(int degree, const MyGenericGraphf* graph, int regionIndex) const{
	MyArrayi degrees;
	if (regionIndex >= 0){
		for (int i = 0; i<mSegmentation[regionIndex].size(); i++){
			degrees << graph->GetNeighbors(mSegmentation[regionIndex][i]).size();
		}
	}
	else{
		for(int i = 0;i<graph->GetNumNodes();i++){
			degrees << graph->GetNeighbors(i).size();
		}
	}
	MyArrayi* sort = degrees.MakeSortResultArray();

	int idx = 0;
	int i;
	for (i = 0; i<sort->size(); i++){
		if(degrees[sort->at(i)] >= degree){
			//idx = sort->at(i);
			break;
		}
	}

	// add some randomization
	if (i >= sort->size()){
		i = sort->size() - 1;
	}
	i += rand() % degree - degree / 2;
	if (i >= sort->size()) i = sort->size() - 1;
	else if (i < 0) i = 0;
	idx = sort->at(i);
	//if(idx<0) idx = 0;
	//if (idx >= degrees.size()) idx = degrees.size()-1;

	delete sort;
	if (regionIndex >= 0){
		return mSegmentation[regionIndex][idx];
	}
	else{
		return idx;
	}
}

int MyGraphTaskGenerator::GetNodePairAboutDegreeDifference(float difference, const MyGenericGraphf* graph1, const MyGenericGraphf* graph2, int regionIndex) const{
	MyArrayi idices;
	if(regionIndex >= 0){
		idices = mSegmentation[regionIndex];
	}
	else{
		idices = MyArrayi::GetSequence(0,graph1->GetNumNodes()-1);
	}
	MyArrayi degrees1;
	MyArrayi degrees2;

	for(int i = 0;i<idices.size();i++){
		int nodeIdx = idices[i];
		degrees1 << graph1->GetNeighbors(nodeIdx).size();
		degrees2 << graph2->GetNeighbors(nodeIdx).size();
	}

	MyArrayf differences;
	for(int i = 0;i<degrees1.size();i++){
		if(degrees1[i] == degrees2[i]){
			differences << 0;
		}
		else if(degrees1[i] > degrees2[i]){
			differences << (degrees1[i] - degrees2[i])/(float)degrees1[i];
		}
		else{
			differences << (degrees2[i] - degrees1[i])/(float)degrees2[i];
		}
	}

	MyArrayi *sort = differences.MakeSortResultArray();

	int possibleIdx;
	for(possibleIdx = 0;possibleIdx<sort->size();possibleIdx++){
		if(differences[possibleIdx] >= difference){
			break;
		}
	}

	possibleIdx += rand()%(NUMNODE/10+1);
	possibleIdx -= rand()%(NUMNODE/10+1);

	Clamp(possibleIdx,0,sort->size()-1);

	return idices[sort->at(possibleIdx)];
	delete sort;
}

MyVec2i MyGraphTaskGenerator::GetNodePairAboutNeighbor(int neighbor, const MyGenericGraphf* graph, int regionIndex) const{
	MyArrayi nodes2comp;
	if (regionIndex >= 0){
		nodes2comp = mSegmentation[regionIndex];
	}
	else{
		nodes2comp = MyArrayi::GetSequence(0,graph->GetNumNodes()-1);
	}
	MyArray2i candicates;
	MyArrayi numCNs;
	for(int i = 0;i<nodes2comp.size();i++){
		int nodei = nodes2comp[i];
		MyArrayi neighbori = graph->GetNeighbors(nodei);
		for(int j = i+1;j<nodes2comp.size();j++){
			int nodej = nodes2comp[j];
			MyArrayi neighborj = graph->GetNeighbors(nodej);
			MyArrayi* commomNeighbors = MyArrayi::MakeCommonElementArray(neighbori,neighborj);
			if(commomNeighbors->size()>1){
				candicates << MyVec2i(nodei,nodej);
				numCNs << commomNeighbors->size();
			}
			delete commomNeighbors;
		}
	}
	MyArrayi* sortInfo = numCNs.MakeSortResultArray();
	int nCandi = sortInfo->size();
	if(nCandi == 0){
		healthy = false;
		return MyVec2i(-1,-1);
	}
	int idx;
	for(idx = 0;idx<nCandi-1;idx++){
		int pairIdx = sortInfo->at(idx);
		if(numCNs[pairIdx] == neighbor){
			break;
		}
	}

	// add some variation
	int variation = rand()%(nCandi/3+1);
	//variation -= nCandi/6;
	idx += variation;
	if(idx<0) idx = 0;
	if(idx>=nCandi) idx = nCandi-1;

	return candicates[sortInfo->at(idx)];

	delete sortInfo;
}

MyVec2i MyGraphTaskGenerator::GetNodePairAboutNeighbor(int neighbor, const MyGenericGraphf* graph, const MyGenericGraphf* graph2, int regionIndex) const{
	if (regionIndex >= 0){
		int numNodes = mSegmentation[regionIndex].size();
		MyArray2i candicates;
		MyArrayi cns;
		for (int i = 0; i < numNodes; i++){
			int candi = mSegmentation[regionIndex][i];
			MyArrayi neighbori = graph->GetNeighbors(candi);
			for (int j = i + 1; j < numNodes; j++){
				int candj = mSegmentation[regionIndex][j];
				int numCommonNeighbors = 0;
				for (int k = 0; k < neighbori.size(); k++){
					int neighborToTest = neighbori[k];
					if (candj != neighborToTest){
						if (graph->GetEdgeIndex(MyVec2i(candj, neighborToTest)) >= 0
							|| graph->GetEdgeIndex(MyVec2i(neighbori[k], candj)) >= 0){
							numCommonNeighbors++;
						}
					}
				}
				//if (numCommonNeighbors == neighbor){
				if (numCommonNeighbors > 0){
					candicates << MyVec2i(candi, candj);
					cns << numCommonNeighbors;
				}
			}
		}

		MyArrayi* sortInfo = cns.MakeSortResultArray();
		int sortIdx = sortInfo->size() - 1 - (rand() % (sortInfo->size() / 4+1));
		if (sortIdx < 0) sortIdx = 0;
		if (sortIdx >= sortInfo->size()) sortIdx = sortInfo->size() - 1;
		int mostCnsIdx = sortInfo->at(sortIdx);
		delete sortInfo;
		//int idx = rand() % candicates.size();
		//return candicates[idx];
		return candicates[mostCnsIdx];
		/*
		MyArray2i newCandidates;
		for (int i = 0; i<candicates.size(); i++){
			int numCommonNeighbors = 0;
			MyArrayi neighbori = graph->GetNeighbors(candicates[i][0]);
			for (int k = 0; k<neighbori.size(); k++){
				int neighborToTest = neighbori[k];
				int otherNode = candicates[i][1];
				if (otherNode != neighborToTest){
					if (graph->GetEdgeIndex(MyVec2i(otherNode, neighborToTest)) >= 0
						|| graph->GetEdgeIndex(MyVec2i(neighbori[k], otherNode)) >= 0){
						numCommonNeighbors++;
					}
				}
			}
			if (numCommonNeighbors >= 0){
				newCandidates << candicates[i];
			}

		}
		int idx = rand() % newCandidates.size();
		return newCandidates[idx];
		*/
	}
	else{
		int numNodes = graph->GetNumNodes();
		MyArray2i candicates;
		for (int i = 0; i<numNodes; i++){
			MyArrayi neighbori = graph->GetNeighbors(i);
			for (int j = i + 1; j<numNodes; j++){
				int numCommonNeighbors = 0;
				for (int k = 0; k<neighbori.size(); k++){
					int neighborToTest = neighbori[k];
					if (j != neighborToTest){
						if (graph->GetEdgeIndex(MyVec2i(j, neighborToTest)) >= 0
							|| graph->GetEdgeIndex(MyVec2i(neighbori[k], j)) >= 0){
							numCommonNeighbors++;
						}
					}
				}
				if (numCommonNeighbors == neighbor){
					candicates << MyVec2i(i, j);
				}
			}
		}

		MyArray2i newCandidates;
		for (int i = 0; i<candicates.size(); i++){
			int numCommonNeighbors = 0;
			MyArrayi neighbori = graph->GetNeighbors(candicates[i][0]);
			for (int k = 0; k<neighbori.size(); k++){
				int neighborToTest = neighbori[k];
				int otherNode = candicates[i][1];
				if (otherNode != neighborToTest){
					if (graph->GetEdgeIndex(MyVec2i(otherNode, neighborToTest)) >= 0
						|| graph->GetEdgeIndex(MyVec2i(neighbori[k], otherNode)) >= 0){
						numCommonNeighbors++;
					}
				}
			}
			if (numCommonNeighbors >= neighbor){
				newCandidates << candicates[i];
			}

		}
		int idx = rand() % newCandidates.size();
		return newCandidates[idx];
	}
}

void MyGraphTaskGenerator::SetMeanMatrixFile(const MyString& file){
	mMeanMatrixFileNames = file;
}

void MyGraphTaskGenerator::SetMatrixFiles(const MyArrayStr& files){
	mMatrixFileNames = files;
}

void MyGraphTaskGenerator::SetHierarchyFile(const MyString& file){
	mHierarchyFileName = file;
}

void MyGraphTaskGenerator::SetPositionFile(const MyString& file){
	mPositionFile = file;
}

void MyGraphTaskGenerator::SetLabelFile(const MyString& file){
	mLabelFile = file;
}

void MyGraphTaskGenerator::ReadSegmentationFile(const MyString& file){
	ifstream segFile;
	segFile.open(file);
	if (!segFile.is_open()){
		cout << "cant open file " << file << endl;
		exit(0);
	}
	mSegmentation.clear();
	MyString comment;
	int numSeg;
	segFile >> numSeg;
	getline(segFile, comment);
	for (int i = 0; i < numSeg; i++){
		int numRegions;
		MyArrayi regions;
		segFile >> numRegions;
		getline(segFile, comment);
		for (int j = 0; j < numRegions; j++){
			int regIdx;
			segFile >> regIdx;
			getline(segFile, comment);
			regions << regIdx;
		}
		mSegmentation << regions;
	}

}

const MyArrayStr& MyGraphTaskGenerator::GetConfigureFiles() const{
	return mConfigureFiles;
}

int MyGraphTaskGenerator::GetNodeOfDegreeCentrality(MyVec2f dcRange, const MyGenericGraphf* graph, int regionIndex){
	MyArrayi candidates;
	if(regionIndex >= 0){
		candidates = mSegmentation[regionIndex];
	}
	else{
		candidates = MyArrayi::GetSequence(0,graph->GetNumNodes()-1);
	}

	MyArrayf dcs;
	MyGraphAlgorithm algor;
	algor.SetGraph(graph);
	MyArrayf alldcs = algor.GetDegreeCentrality();

	for(int i = 0;i<candidates.size();i++){
		dcs << alldcs[candidates[i]];
	}

	MyArrayi qualified;

	for(int i = 0;i<dcs.size();i++){
		if(dcs[i] >= dcRange[0] && dcs[i] <= dcRange[1]){
			qualified << i;
		}
	}

	if(qualified.empty()){
		mHealthy = false;
		return -1;
	}

	int idxq = rand()%qualified.size();

	return candidates[qualified[idxq]];
}

int MyGraphTaskGenerator::GetNodeOfDegreeCentralityDifference(MyVec2f dcDiffRange, const MyGenericGraphf* graph1, const MyGenericGraphf* graph2, int regionIndex){
	MyArrayi candidates;
	if(regionIndex >= 0){
		candidates = mSegmentation[regionIndex];
	}
	else{
		candidates = MyArrayi::GetSequence(0,graph1->GetNumNodes()-1);
	}

	MyGraphAlgorithm algor;
	algor.SetGraph(graph1);
	MyArrayf dc_1 = algor.GetDegreeCentrality();
	algor.SetGraph(graph2);
	MyArrayf dc_2 = algor.GetDegreeCentrality();

	MyArrayf dc_diff;
	for(int i = 0;i<candidates.size();i++){
		dc_diff << dc_2[candidates[i]] - dc_1[candidates[i]];

		if(dc_diff[i]<0) dc_diff[i] = -dc_diff[i];
	}

	MyArrayi qualified;

	for(int i = 0;i<dc_diff.size();i++){
		if(dc_diff[i] >= dcDiffRange[0] && dc_diff[i] <= dcDiffRange[1]){
			qualified << i;
		}
	}

	if(qualified.empty()){
		mHealthy = false;
		return -1;
	}

	int idxq = rand()%qualified.size();

	return candidates[qualified[idxq]];
}

MyVec2i MyGraphTaskGenerator::GetNodePairOfCommonNeighbor(MyVec2i neighborRange, const MyGenericGraphf* graph, int regionIndex){
	MyArrayi candidates;
	if(regionIndex >= 0){
		candidates = mSegmentation[regionIndex];
	}
	else{
		candidates = MyArrayi::GetSequence(0,graph->GetNumNodes()-1);
	}

	MyArray2i qualified;
	for(int i = 0;i<candidates.size();i++){
		int nodeIdxi = candidates[i];
		MyArrayi neighbori = graph->GetNeighbors(nodeIdxi);
		for(int j = i+1;j<candidates.size();j++){
			int nodeIdxj = candidates[j];
			MyArrayi neighborj = graph->GetNeighbors(nodeIdxj);
			MyArrayi *commonNeighbor = MyArrayi::MakeCommonElementArray(neighbori, neighborj);
			if(commonNeighbor->size() >= neighborRange[0]
				&& commonNeighbor->size() <= neighborRange[1]){
				qualified << MyVec2i(nodeIdxi, nodeIdxj);
			}
			delete commonNeighbor;
		}
	}

	if(qualified.empty()){
		mHealthy = false;
		return MyVec2i(-1,-1);
	}

	int idxq = rand()%qualified.size();

	return qualified[idxq];
}

int MyGraphTaskGenerator::GetNodeOfHubNeighbor(float hubThres,  MyVec2i neighborRange, const MyGenericGraphf* graph, int regionIndex){
	MyArrayi candidates;
	if(regionIndex >= 0){
		candidates = mSegmentation[regionIndex];
	}
	else{
		candidates = MyArrayi::GetSequence(0,graph->GetNumNodes()-1);
	}

	MyGraphAlgorithm algor;
	algor.SetGraph(graph);
	algor.FloydWarshallWithPathReconstruction();
	MyArrayf bets = algor.GetBetweennessCentrality();

	MyArrayi hubNodes;
	for(int i = 0;i<graph->GetNumNodes();i++){
		if(bets[i] >= hubThres){
			hubNodes << i;
		}
	}

	MyArrayb isNodeQualify(graph->GetNumNodes(), false);
	for(int i = 0;i<hubNodes.size();i++){
		int hubNode = hubNodes[i];
		MyArrayi hubNodeNeighbors = graph->GetNeighbors(hubNode);
		for(int j = 0;j<hubNodeNeighbors.size();j++){
			int hubNodeNeighbor = hubNodeNeighbors[j];
			isNodeQualify[hubNodeNeighbor] = true;
		}
	}

	MyArrayi qualified;
	for(int i = 0;i<candidates.size();i++){
		int nodeIdx = candidates[i];
		if(isNodeQualify[nodeIdx]){
			int thisNodeNeighbor = graph->GetNeighbors(nodeIdx).size();
			if(thisNodeNeighbor>=neighborRange[0] && thisNodeNeighbor <= neighborRange[1]){
				qualified << nodeIdx;
			}
		}
	}
	
	if(qualified.size()<=0){
		mHealthy = false;
		return -1;
	}
	int idxq = rand()%qualified.size();

	// make sure the max 2 has least difference
	float AllMaxBt = bets[bets.GetMaximumIndex()];
	MyArrayi thisNodeNeighbors = graph->GetNeighbors(qualified[idxq]);
	float maxBt = -1;
	int maxBtIdx = -1;
	for(int i = 0;i<thisNodeNeighbors.size();i++){
		int thisNodeNeighbor = thisNodeNeighbors[i];
		float thisNodeNeighborBt = bets[i];
		if(thisNodeNeighborBt > maxBt){
			maxBt = thisNodeNeighborBt;
			maxBtIdx = thisNodeNeighbor;
		}
	}
	for(int i = 0;i<thisNodeNeighbors.size();i++){
		int thisNodeNeighbor = thisNodeNeighbors[i];
		if(thisNodeNeighbor != maxBtIdx){
			float thisNodeNeighborBt = bets[i];
			if(maxBt-thisNodeNeighborBt <= MINIMALDIFF*AllMaxBt){
				mHealthy = false;
				return -1;
			}
		}
	}

	return qualified[idxq];
}

int MyGraphTaskGenerator::GetOppositeNodeOfHubNeighbor(float hubThres,  MyVec2i neighborRange, const MyGenericGraphf* graph, int regionIndex){
	MyArrayi candidates;
	if(regionIndex >= 0){
		candidates = mSegmentation[regionIndex];
	}
	else{
		candidates = MyArrayi::GetSequence(0,graph->GetNumNodes()-1);
	}

	MyGraphAlgorithm algor;
	algor.SetGraph(graph);
	algor.FloydWarshallWithPathReconstruction();
	MyArrayf bets = algor.GetBetweennessCentrality();

	MyArrayi hubNodes;
	for(int i = 0;i<graph->GetNumNodes();i++){
		if(bets[i] >= hubThres){
			hubNodes << i;
		}
	}

	MyArrayStr* label = MyDataLoader::MakeArrayStrFromFile(mLabelFile);

	MyArrayb isNodeQualify(graph->GetNumNodes(), false);
	for(int i = 0;i<hubNodes.size();i++){
		int hubNode = hubNodes[i];
		int side = this->getSide(label->at(hubNode));
		MyArrayi hubNodeNeighbors = graph->GetNeighbors(hubNode);
		for(int j = 0;j<hubNodeNeighbors.size();j++){
			int hubNodeNeighbor = hubNodeNeighbors[j];
			int hubNodeNeighborSide = this->getSide(label->at(hubNodeNeighbor));
			if(side != hubNodeNeighborSide){
				isNodeQualify[hubNodeNeighbor] = true;
			}
		}
	}

	MyArrayi qualified;
	for(int i = 0;i<candidates.size();i++){
		int nodeIdx = candidates[i];
		if(isNodeQualify[nodeIdx]){
			int thisNodeNeighbor = graph->GetNeighbors(nodeIdx).size();
			if(thisNodeNeighbor>=neighborRange[0] && thisNodeNeighbor <= neighborRange[1]){
				qualified << nodeIdx;
			}
		}
	}
	
	if(qualified.size() <= 0){
		mHealthy = false;
		delete label;
		return -1;
	}
	int idxq = rand()%qualified.size();

	// make sure the max 2 has least difference
	float AllMaxBt = bets[bets.GetMaximumIndex()];
	MyArrayi thisNodeNeighbors = graph->GetNeighbors(qualified[idxq]);
	float maxBt = -1;
	int maxBtIdx = -1;
	for(int i = 0;i<thisNodeNeighbors.size();i++){
		int thisNodeNeighbor = thisNodeNeighbors[i];
		float thisNodeNeighborBt = bets[i];
		if(thisNodeNeighborBt > maxBt){
			maxBt = thisNodeNeighborBt;
			maxBtIdx = thisNodeNeighbor;
		}
	}
	for(int i = 0;i<thisNodeNeighbors.size();i++){
		int thisNodeNeighbor = thisNodeNeighbors[i];
		if(thisNodeNeighbor != maxBtIdx){
			float thisNodeNeighborBt = bets[i];
			if(maxBt-thisNodeNeighborBt <= MINIMALDIFF*AllMaxBt){
				mHealthy = false;
				return -1;
			}
		}
	}

	delete label;
	return qualified[idxq];
}

int MyGraphTaskGenerator::GetLobeThatHasHub(float hubThres, const MyGenericGraphf* graph, const MyGenericNode* hierarchy){
	int numNode = graph->GetNumNodes();
	int numLobe = hierarchy->GetNumChildren();
	MyArrayi node2cluster(numNode,-1);
	for(int i = 0;i<numLobe;i++){
		MyArrayStr nodeIdxStrs;
		MyGenericHierarchyOverlayStr::AddLeaveObjectToArray(&nodeIdxStrs,hierarchy->GetChild(i));
		for(int inode = 0;inode<nodeIdxStrs.size();inode++){
			int nodeIdx = nodeIdxStrs[inode].ToInt();
			node2cluster[nodeIdx] = i;
		}
	}
	for (int i = 0; i < node2cluster.size(); i++){
		assert(node2cluster[i] >= 0);
	}

	MyArrayb lobeQualified(numLobe, false);
	MyGraphAlgorithm algor;
	algor.SetGraph(graph);
	algor.FloydWarshallWithPathReconstruction();
	MyArrayf btns = algor.GetBetweennessCentrality();
	for(int i = 0;i<numNode;i++){
		if(btns[i] > hubThres){
			int lobe = node2cluster[i];
			lobeQualified[lobe] = true;
		}
	}

	MyArrayi candidates;
	for(int i = 0;i<numLobe;i++){
		if(lobeQualified[i]){
			candidates << i;
		}
	}

	if(candidates.size() <= 0){
		mHealthy = false;
		return -1;
	}
	
	int idx = rand()%candidates.size();

	// make sure the max 2 has least difference
	float AllMaxBt = btns[btns.GetMaximumIndex()];
	float maxBt = -1;
	int maxBtIdx = -1;
	for(int i = 0;i<node2cluster.size();i++){
		if(node2cluster[i] == candidates[idx]){
			if(btns[i]>maxBt){
				maxBt = btns[i];
				maxBtIdx = i;
			}
		}
	}
	for(int i = 0;i<node2cluster.size();i++){
		if(node2cluster[i] == candidates[idx] && i!=maxBtIdx){
			if(maxBt-btns[i]<MINIMALDIFF*AllMaxBt){
				mHealthy = false;
				return -1;
			}
		}
	}

	return candidates[idx];
}

MyVec2i MyGraphTaskGenerator::GetInterLobeLinkDifference(MyVec2i intLobDiffRange,
														 const MyGenericGraphf* graph1,
														 const MyGenericGraphf* graph2, 
														const MyGenericNode* hierarchy){
	int numNode = graph1->GetNumNodes();
	int numLobe = hierarchy->GetNumChildren();
	MyArrayi node2cluster(numNode,-1);
	for(int i = 0;i<numLobe;i++){
		MyArrayStr nodeIdxStrs;
		MyGenericHierarchyOverlayStr::AddLeaveObjectToArray(&nodeIdxStrs,hierarchy->GetChild(i));
		for(int inode = 0;inode<nodeIdxStrs.size();inode++){
			int nodeIdx = nodeIdxStrs[inode].ToInt();
			node2cluster[nodeIdx] = i;
		}
	}
	for (int i = 0; i < node2cluster.size(); i++){
		assert(node2cluster[i] >= 0);
	}

	MyMatrixi interLobeLinks1 (numLobe,numLobe, 0);
	for(int iedge = 0;iedge <graph1->GetNumEdges();iedge++){
		MyVec2i edge = graph1->GetEdge(iedge);
		MyVec2i edgeCluster(node2cluster[edge[0]],node2cluster[edge[1]]);
		interLobeLinks1.At(edgeCluster[0],edgeCluster[1]) ++;
		interLobeLinks1.At(edgeCluster[1],edgeCluster[0]) ++;
	}
	MyMatrixi interLobeLinks2 (numLobe,numLobe, 0);
	for(int iedge = 0;iedge <graph2->GetNumEdges();iedge++){
		MyVec2i edge = graph2->GetEdge(iedge);
		MyVec2i edgeCluster(node2cluster[edge[0]],node2cluster[edge[1]]);
		interLobeLinks2.At(edgeCluster[0],edgeCluster[1]) ++;
		interLobeLinks2.At(edgeCluster[1],edgeCluster[0]) ++;
	}

	MyMatrixi diffLinks = interLobeLinks1 - interLobeLinks2;

	MyArray2i cadidates;
	for(int i = 0;i<numLobe;i++){
		for(int j = i+1;j<numLobe;j++){
			int diff = diffLinks.At(i,j);
			if(diff<0) diff = -diff;
			if(diff >= intLobDiffRange[0] && diff <= intLobDiffRange[1]){
				cadidates << MyVec2i(i,j);
			}
		}
	}

	if(cadidates.size()<=0){
		mHealthy = false;
		return MyVec2i(-1,-1);
	}

	int idx = rand()%cadidates.size();

	return cadidates[idx];
}

MyVec2i MyGraphTaskGenerator::GetAreasWithVariationDifference(MyVec2i linkChangeDiffRange, 
													const MyGenericGraphf* graph0, 
													const MyGenericGraphf* graph1, 
													const MyGenericNode* hierarchy){
	MyArrayi node2lobe(graph0->GetNumNodes(), -1);
	for (int i = 0; i<hierarchy->GetNumChildren(); i++){
		const MyGenericNode* child = hierarchy->GetChild(i);
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

	MyArrayi lobeNumEdge0(hierarchy->GetNumChildren(), 0);
	MyArrayi lobeNumEdge1(hierarchy->GetNumChildren(), 0);

	for (int i = 0; i < graph0->GetNumEdges(); i++){
		MyVec2i edge = graph0->GetEdge(i);
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

	for (int i = 0; i < graph1->GetNumEdges(); i++){
		MyVec2i edge = graph1->GetEdge(i);
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
	for (int i = 0; i < hierarchy->GetNumChildren(); i++){
		int change = lobeNumEdge0[i] - lobeNumEdge1[i];
		if(change <0) change = -change;
		variations << change;
	}

	MyArray2i candicates;
	for(int i = 0;i<variations.size();i++){
		for(int j = i+1;j<variations.size();j++){
			int variationDiff = abs(variations[i]-variations[j]);
			if(variationDiff >= linkChangeDiffRange[0]
			&& variationDiff <= linkChangeDiffRange[1]){
				candicates << MyVec2i(i,j);
			}
		}
	}

	if(candicates.size()<=0){
		mHealthy = false;
		return MyVec2i(-1,-1);
	}
	int idx = rand()%candicates.size();
	return candicates[idx];
}

MyArrayi MyGraphTaskGenerator::ComputeExtraInfoAtDifficulty(const MyString& task, const VisSpec& spec, const MyString& matfile, const MyString& matfile2, int difficulty, int regionIndex){
	MyGenericGraphf* graph = MyDataLoader::MakeGraphFromMatrixFileFilterByEdgeRatio(matfile,NUMNODE,NUMNODE,spec.GetDensityValue());
	int densityOffset = 0;
	if(spec.GetDensityValue()>0.06) densityOffset = 3;
	MyArrayi rst;
	if(task == "task_degree"){
		return MyArrayi(1,this->GetNodeOfDegreeCentrality(mMeanGraphDegreeSegs[difficulty+densityOffset], graph, regionIndex));
	}
	else if(task == "task_degree_compare"){
		MyGenericGraphf* graph2 = MyDataLoader::MakeGraphFromMatrixFileFilterByEdgeRatio(matfile2,NUMNODE,NUMNODE,spec.GetDensityValue());
		rst = MyArrayi(1,this->GetNodeOfDegreeCentralityDifference(mDegreeDiffSegs[difficulty+densityOffset], graph, graph2, regionIndex));
		delete graph2;
	}
	else if(task == "task_neighbor"){
		//MyGenericGraphf* graph2 = MyDataLoader::MakeGraphFromMatrixFileFilterByEdgeRatio(matfile2,NUMNODE,NUMNODE,spec.GetDensityValue());
		MyVec2i nodes = this->GetNodePairOfCommonNeighbor(mCommonNeighborSegs[difficulty+densityOffset], graph, regionIndex);
		rst << nodes[0] << nodes[1];
	}
	else if (task == "task_variation"){
		MyGenericGraphf* graph2 = MyDataLoader::MakeGraphFromMatrixFileFilterByEdgeRatio(matfile2,NUMNODE,NUMNODE,spec.GetDensityValue());
		MyGenericNode* hierarchy = MyDataLoader::LoadHierarchyOverlay(mHierarchyFileName);
		MyVec2i areas = this->GetAreasWithVariationDifference(mLinkChangeDiffRange[difficulty+densityOffset], graph, graph2,hierarchy);
		delete graph2;
		hierarchy->DestoryAll();
		delete hierarchy;
		rst << areas[0] << areas[1];
	}
	else if (task == "task_hub"){
		rst = MyArrayi(1, this->GetNodeOfHubNeighbor(mMeanGraphHubNodeThres[difficulty+densityOffset], mNeighborRange[difficulty+densityOffset], graph, regionIndex));
	}
	else if (task == "task_inter_lobe"){
		MyGenericNode* hierarchy = MyDataLoader::LoadHierarchyOverlay(mHierarchyFileName);MyGenericGraphf* graph2 = MyDataLoader::MakeGraphFromMatrixFileFilterByEdgeRatio(matfile2,NUMNODE,NUMNODE,spec.GetDensityValue());
		MyVec2i interLobe = this->GetInterLobeLinkDifference(mInterLobeLinkDiffRange[difficulty+densityOffset],graph,graph2, hierarchy);
		rst << interLobe[0] << interLobe[1];
		delete hierarchy;
		delete graph2;
	}
	else if (task == "task_lobe_hub"){
		MyGenericNode* hierarchy = MyDataLoader::LoadHierarchyOverlay(mHierarchyFileName);
		int lobe = this->GetLobeThatHasHub(mMeanGraphHubNodeThres[difficulty+densityOffset], graph, hierarchy);
		rst << lobe;
		delete hierarchy;
	}
	else if (task == "task_active_lobe"){
		MyGenericNode* hierarchy = MyDataLoader::LoadHierarchyOverlay(mHierarchyFileName);
		for(int i = 0;i<hierarchy->GetNumChildren();i++){
			MyString name = dynamic_cast<MyGenericHierarchyOverlayStr*>(hierarchy->GetChild(i))->GetObject();
			if(name.back() == 'L' || name.back() == 'l'){
				rst << i;
			}
		}
		delete hierarchy;
		
	}
	else if (task == "task_opposite_hub"){
		rst = MyArrayi(1, this->GetOppositeNodeOfHubNeighbor(mMeanGraphHubNodeThres[difficulty+densityOffset], mOppositeNeighborRange[difficulty+densityOffset], graph, regionIndex));
	}
	delete graph;
	return rst;
}

int MyGraphTaskGenerator::getSide(MyString name) const{
	if(name.back() == 'L' || name.back() == 'l'){
		return 0;
	}
	return 1;
}