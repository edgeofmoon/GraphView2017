#pragma once
#include "mygenericgraph.h"
#include "MyArray.h"
#include "MyVec.h"

template<typename T>
class MyGraphEdgeNodeList :
	public MyGenericGraph<T>
{
public:
	MyGraphEdgeNodeList(void);
	~MyGraphEdgeNodeList(void);

	virtual void Copy(const MyGenericGraph<T>& other) = 0;

	// depth copy function
	//virtual MyGenericGraph<T>& operator=(const MyGenericGraph<T>& graph);

	// if no edge between nodes exists, return -1;
	virtual int GetEdgeIndex(const MyVec2i& edges) const = 0;
	virtual MyVec2i GetEdge(int i) const = 0;

	// all rest code index edge with i only
	virtual int GetNumEdges() const = 0;
	virtual int GetNumNodes() const = 0;
  

	// does not check if edge/node exists or not
	virtual const T GetEdgeWeight(int i) const = 0;
	virtual const T GetNodeWeight(int i) const = 0;
	
	// do nothing if edge/node does not exists
	virtual void SetNodeWeight(T weight, int nodeIdx) = 0;
	virtual void SetEdgeWeight(T weight, int edgeIdx) = 0;

protected:
	MyArrayi* mNodes;
	MyArray2i* mEdges;
	MyArray<T>* mNodeWeights;
	MyArray<T>* mEdgeWeights;
};

template<typename T>
MyGraphEdgeNodeList<T>::MyGraphEdgeNodeList(void){
	mNodes = new MyArrayi;
	mEdges = new MyArray2i;
	mNodeWeights = new MyArray<T>;
	mEdgeWeights = new MyArray<T>;
}

template<typename T>
MyGraphEdgeNodeList<T>::~MyGraphEdgeNodeList(void){
	delete mNodes;
	delete mEdges;
	delete mNodeWeights;
	delete mEdgeWeights;
}

template<typename T>
void MyGraphEdgeNodeList<T>::Copy(const MyGenericGraph<T>& other){

}

template<typename T>
int MyGraphEdgeNodeList<T>::GetEdgeIndex(const MyVec2i& edges) const{
}

template<typename T>
MyVec2i MyGraphEdgeNodeList<T>::GetEdge(int i) const{
}

template<typename T>
int MyGraphEdgeNodeList<T>::GetNumEdges() const{
}

template<typename T>
int MyGraphEdgeNodeList<T>::GetNumNodes() const{
}
  
template<typename T>
const T MyGraphEdgeNodeList<T>::GetEdgeWeight(int i){

}

template<typename T>
const T MyGraphEdgeNodeList<T>::GetNodeWeight(int i) const{

}
	
template<typename T>
void MyGraphEdgeNodeList<T>::SetNodeWeight(T weight, int nodeIdx){

}

template<typename T>
void MyGraphEdgeNodeList<T>::SetEdgeWeight(T weight, int edgeIdx){

}