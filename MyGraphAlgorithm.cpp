#include "MyGraphAlgorithm.h"
#include "MyUtility.h"
#include "MyDenseGraph.h"
#include "svd.h"
#include <fstream>
using namespace std;
#define INFINITY 99999999999

#define BINARY_BIT 0x01
#define DIRECTIONAL_BIT 0x02

MyGraphAlgorithm::MyGraphAlgorithm(void)
{
	mGraph = 0;
	mFlag = BINARY_BIT;
}


MyGraphAlgorithm::~MyGraphAlgorithm(void)
{
}

void MyGraphAlgorithm::SetGraph(const MyGenericGraphf* graph){
	mGraph = graph;
}

void MyGraphAlgorithm::FloydWarshallWithPathReconstruction(){
	int n = mGraph->GetNumNodes();
	mDistance = MyMatrixf(n,n,INFINITY);
	for(int i = 0;i<n;i++){
		for(int j = 0;j<n;j++){
			int edgeIdx = mGraph->GetEdgeIndex(MyVec2i(i,j));
			int edgeIdx2 = mGraph->GetEdgeIndex(MyVec2i(j,i));
			if(edgeIdx >= 0 || edgeIdx2>=0){
				if(this->IsBinary()){
					mDistance.At(i,j) = 1.0;
				}
				else{
					mDistance.At(i,j) = mGraph->GetEdgeWeight(edgeIdx);
				}
			}
		}
	}
	/*
	ofstream mat;
	mat.open("mat.txt");
	for(int i = 0;i<mDistance.GetNumRows();i++){
		for(int j = 0;j<mDistance.GetNumCols();j++){
			mat << mDistance.At(i,j) << ' ';
		}
		mat << endl;
	}
	*/
	this->initNextArray();

	for(int k = 0;k<n;k++){
		for(int i = 0;i<n;i++){
			for(int j = 0;j<n;j++){
				if(mDistance.At(i,k)+mDistance.At(k,j)<mDistance.At(i,j)){
					mDistance.At(i,j) = mDistance.At(i,k)+mDistance.At(k,j);
					mNext.At(i,j) = mNext.At(k,j);
				}
			}
		}
	}
}

MyArrayi MyGraphAlgorithm::GetShortestPath(int i, int j) const{
	if(mDistance.At(i,j) == INFINITY){
		// no path
		return MyArrayi();
	}
	int intermediate = mNext.At(i,j);
	if(intermediate == i){
		// the direct edge from i to j gives the shortest path
		MyArrayi rst;
		rst << i << j;
		return rst;
	}
	else{
		MyArrayi a1 = this->GetShortestPath(i, intermediate);
		MyArrayi a2 = this->GetShortestPath(intermediate,j);
		if(!a1.empty()) a1.pop_back();
		a1.PushBack(&a2);
		return a1;
	}
}

float MyGraphAlgorithm::GetShortestPathLength(int i, int j) const{
	return mDistance.At(i,j);
}

MyArrayf MyGraphAlgorithm::GetBetweennessCentrality() const{
	int n = mGraph->GetNumNodes();
	MyMatrixi numPaths(n,n,0);
	for(int i = 0;i<n;i++){
		for(int j = i+1;j<n;j++){
			MyArrayi path = this->GetShortestPath(i,j);
			if(path.empty()) continue;
			for(int k = 0; k<path.size()-1; k++){
				int ai = path[k];
				int bi = path[k+1];
				numPaths.At(ai,bi) ++;
			}
		}
	}
	MyArrayf bet(n,0);
	for(int i = 0;i<n;i++){
		for(int j = 0;j<n;j++){
			bet[i] += numPaths.At(i,j);
		}
		//normalization
		bet[i] /= (n*(n-1));
	}
	return bet;
}

MyArrayf MyGraphAlgorithm::GetDegreeCentrality() const{
	int n = mGraph->GetNumNodes();
	MyArrayf deg(n,0);
	for(int i = 0;i<n;i++){
		//normalization
		deg[i] = mGraph->GetNeighbors(i).size()/(float)(n-1);
	}
	return deg;
}

MyArrayf MyGraphAlgorithm::GetEigenvectorCentrality() const{
	int n = mGraph->GetNumNodes();
	float **mat = new float*[n];
	for(int i = 0;i<n;i++){
		mat[i] = new float[n];
	}

	for(int i = 0;i<n;i++){
		for(int j = 0;j<n;j++){
			if(mGraph->GetEdgeIndex(MyVec2i(i,j))>=0 || i == j 
				|| mGraph->GetEdgeIndex(MyVec2i(j,i))>=0){
				mat[i][j]=1;
			}
			else{
				mat[i][j]=0;
			}
		}
	}
	/*
	ofstream matfile;
	matfile.open("mat.txt");
	for(int i = 0;i<n;i++){
		for(int j = 0;j<n;j++){
			matfile << mat[i][j] << ' ';
		}
		matfile << endl;
	}
	*/
	float **mat2 = new float*[n];
	for(int i = 0;i<n;i++){
		mat2[i] = new float[n];
	}
	float *eigens = new float[n];
	int c = dsvd(mat,n,n,eigens,mat2);

	MyArrayf ev(n);
	for(int i = 0;i<n;i++){
		ev[i] = eigens[i];
	}

	MyArrayi* sortInfo = ev.MakeSortResultArray();
	int largestEgv = sortInfo->back();
	delete sortInfo;

	MyArrayf rst(n);
	for(int i = 0;i<n;i++){
		rst[i] = mat2[largestEgv][i];
	}
	
	for(int i = 0;i<n;i++){
		delete mat[i];
		delete mat2[i];
	}
	delete mat;
	delete mat2;
	delete eigens;

	return rst;
}

MyArrayi MyGraphAlgorithm::GetTopDegreeNodes(int num2get) const{
	int n = mGraph->GetNumNodes();
	MyArrayf deg = this->GetDegreeCentrality();
	MyArrayi* sortRst = deg.MakeSortResultArray();
	MyArrayi rst;
	for(int i = 0;i<num2get && i<sortRst->size();i++){
		rst << sortRst->at(i);
	}
	delete sortRst;
	return rst;
}

void MyGraphAlgorithm::initNextArray(){
	int n = mGraph->GetNumNodes();
	mNext = MyMatrixi(n,n);
	for(int i = 0;i<n;i++){
		for(int j = 0;j<n;j++){
			if(i == j || mDistance.At(i,j) == INFINITY){
				mNext.At(i,j) = 0;
			}
			else{
				mNext.At(i,j) = i;
			}
		}
	}
}


void MyGraphAlgorithm::SetAsBinaryGraph(bool b){
	mFlag |= BINARY_BIT;
}

bool MyGraphAlgorithm::IsBinary(){
	return (mFlag & BINARY_BIT != 0);
}

MyGenericGraphf* MyGraphAlgorithm::MakeDifferenceGraph(const MyGenericGraphf* newGraph) const{
	int numNodes = mGraph->GetNumNodes();
	assert(numNodes == newGraph->GetNumNodes());
	MyDenseGraphf *diffGraph = new MyDenseGraphf(numNodes);

	MyArray2i edges0 = mGraph->GetEdges();
	MyArray2i edges1 = newGraph->GetEdges();
	MyArray2i* sameEdges = MyArray2i::MakeCommonElementArray(edges0,edges1);
	MyArray2i compleEdges = edges0;
	compleEdges += edges1;
	compleEdges.EraseOne(sameEdges);

	for(int i = 0;i<compleEdges.size();i++){
		float weight = 1;
		int idx0 = mGraph->GetEdgeIndex(compleEdges[i]);
		if(idx0 >= 0){
			weight = mGraph->GetEdgeWeight(idx0);
		}
		else{
			int idx1 = newGraph->GetEdgeIndex(compleEdges[i]);
			assert(idx1 >= 0);
			weight = newGraph->GetEdgeWeight(idx1);
		}
		diffGraph->AddEdge(compleEdges[i],weight);
	}

	return diffGraph;
}

#undef INFINITY
#undef BINARY_BIT
#undef DIRECTIONAL_BIT