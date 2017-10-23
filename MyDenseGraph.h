#pragma once

#include "MyGenericGraph.h"
#include "MyMatrix.h"

#include "MyArray.h"

template<typename T>
class MyDenseGraph
	:public MyGenericGraph<T>
{
public:
	MyDenseGraph();
	MyDenseGraph(MyMatrix<T>* mat);
	MyDenseGraph(int nNodes);
	~MyDenseGraph(void);

	void SetMatrix(MyMatrix<T>* mat);
	const MyMatrix<T>* GetMatrix() const {return mMat;};
	
	virtual void Copy(const MyGenericGraph<T>& other);
	virtual MyVec2i GetEdge(int i) const;

	virtual int GetEdgeIndex(const MyVec2i& nodes) const;
	virtual int GetNumEdges() const;
	virtual int GetNumNodes() const;
  
	virtual const T GetEdgeWeight(int index) const;
	virtual const T GetNodeWeight(int index) const;

	virtual void SetNodeWeight(T weight, int nodeIdx);
	virtual void SetEdgeWeight(T weight, int edgeIdx);
	
	virtual MyArray2i GetEdges() const;
	
	virtual void AddEdge(const MyVec2i& edges, T weight = 1);
	virtual void DeleteEdge(int edgeIdx);
	
	virtual void LeaveOnlyNodes(MyArrayi nodes);
	virtual void LeaveOnlyEdges(MyArrayi edges);

protected:
	MyMatrix<T>* mMat;

	void updateEdges();
	MyArray2i mEdges;
};

template<typename T>
MyDenseGraph<T>::MyDenseGraph()
{
	mMat = 0;
}

template<typename T>
MyDenseGraph<T>::MyDenseGraph(MyMatrix<T>* mat)
{
	this->SetMatrix(mat);
}

template<typename T>
MyDenseGraph<T>::MyDenseGraph(int nNodes){
	mMat = new MyMatrixf(nNodes,nNodes);
}
template<typename T>
MyDenseGraph<T>::~MyDenseGraph(void)
{
	if (mMat) delete mMat;
}

template<typename T>
void MyDenseGraph<T>::SetMatrix(MyMatrix<T>* mat){
	if (mMat) delete mMat;
	mMat = mat;
	this->updateEdges();
}

template<typename T>
void MyDenseGraph<T>::Copy(const MyGenericGraph<T>& other){
	if (mMat) delete mMat;
	mMat = new MyMatrixf(other.GetNumNodes(),other.GetNumNodes());
	this->AddEdges(other.GetEdges());
	for(int i = 0;i<other.GetNumEdges();i++){
		this->SetEdgeWeight(other.GetEdgeWeight(i),i);
	}
	for(int i = 0;i<other.GetNumEdges();i++){
		this->SetEdgeWeight(other.GetEdgeWeight(i),i);
	}
}

template<typename T>
int MyDenseGraph<T>::GetNumEdges() const{
	return mEdges.size();
}

template<typename T>
int MyDenseGraph<T>::GetNumNodes() const{
	return mMat->GetNumRows();
}

template<typename T>
MyVec2i MyDenseGraph<T>::GetEdge(int i) const{
	return mEdges[i];
}

template<typename T>
int MyDenseGraph<T>::GetEdgeIndex(const MyVec2i& nodes) const{
	for(int i = 0;i<mEdges.size();i++){
		if(mEdges[i] == nodes){
			return i;
		}
	}
	return -1;
}

template<typename T>
const T MyDenseGraph<T>::GetEdgeWeight(int index) const{
	MyVec2i idx2 = this->GetEdge(index);  
	return mMat->At(idx2[0],idx2[1]);
}

template<typename T>
const T MyDenseGraph<T>::GetNodeWeight(int index) const{
	return mMat->At(index,index);
}
	
template<typename T>
void MyDenseGraph<T>::SetNodeWeight(T weight, int nodeIdx){
	mMat->At(nodeIdx,nodeIdx) = weight;
}

template<typename T>
void MyDenseGraph<T>::SetEdgeWeight(T weight, int edgeIdx){
	MyVec2i nodeIdx = this->GetEdge(edgeIdx);
	mMat->At(nodeIdx[0],nodeIdx[1]) = weight;
}

template<typename T>
void MyDenseGraph<T>::LeaveOnlyNodes(MyArrayi nodes){
	mMat = new MyMatrixf((mMat->GetCols(nodes)).GetRows(nodes));
	this->updateEdges();
}

template<typename T>
MyArray2i MyDenseGraph<T>::GetEdges() const{
	return mEdges;
}

template<typename T>
void MyDenseGraph<T>::AddEdge(const MyVec2i& edges, T weight){
	mEdges.push_back(edges);
	mMat->At(edges[0],edges[1]) = weight;
}

template<typename T>
void MyDenseGraph<T>::DeleteEdge(int edgeIdx){
	MyVec2i edge = this->GetEdge(edgeIdx);
	mMat->At(edge[0],edge[1]) = 0;
	mEdges.EraseAt(edgeIdx);
}

template<typename T>
void MyDenseGraph<T>::LeaveOnlyEdges(MyArrayi edges){
	MyMatrixf maskMat(this->GetNumNodes(),this->GetNumNodes(),0.f);
	for(int i = 0;i<edges.size();i++){
		MyVec2i nodes = this->GetEdge(edges[i]);
		maskMat.At(nodes[0],nodes[1]) = 1.0f;
	}
	MyMatrixf* mat = new MyMatrixf(mMat->DotMultiply(maskMat));
	if(mMat) delete mMat;
	mMat = mat;
	MyArrayi nodes = this->GetNodesByEdges(edges);
	this->LeaveOnlyNodes(nodes);
}

template<typename T>
void MyDenseGraph<T>::updateEdges(){
	mEdges.clear();
	for(int i = 0;i<mMat->GetNumRows();i++){
		for(int j = i+1;j<mMat->GetNumCols();j++){
			if(mMat->At(i,j) != 0){
				mEdges.push_back(MyVec2i(i,j));
			}
		}
	}
}

typedef MyDenseGraph<float> MyDenseGraphf;
