#pragma once

#include "MyVec.h"
#include "MyGenericSelectableDataAbstract.h"

#include "MyArray.h"

template<typename T>
class MyGenericGraph
	:public MyGenericSelectableDataAbstract
{
public:
	MyGenericGraph(){};

	const static int GRAPH_NODE = 1;
	const static int GRAPH_EDGE = 2;

	const static int GRAPH_EDGE_NAME_MASK = 0x0000ffff;

	static MyVec2i DecodeEdge(int edgeName);
	static int EncodeEdge(MyVec2i edge);

	// start an empty graph with nNodes
	// nodes space allocated, edge space not guaranteed
	MyGenericGraph(int nNodes){};

	// depth copy construct
	// alow graph transformation
	MyGenericGraph(const MyGenericGraph<T>& graph);
	virtual ~MyGenericGraph(void){};

	virtual void Copy(const MyGenericGraph<T>& other) = 0;

	// depth copy function
	//virtual MyGenericGraph<T>& operator=(const MyGenericGraph<T>& graph);

	// if no edge between nodes exists, return -1;
	virtual int GetEdgeIndex(const MyVec2i& edges) const = 0;
	virtual MyVec2i GetEdge(int index) const = 0;

	// all rest code index edge with i only
	virtual int GetNumEdges() const = 0;
	virtual int GetNumNodes() const = 0;
  

	// does not check if edge/node exists or not
	virtual const T GetEdgeWeight(int index) const = 0;
	virtual const T GetNodeWeight(int index) const = 0;
	
	// do nothing if edge/node does not exists
	virtual void SetNodeWeight(T weight, int nodeIdx) = 0;
	virtual void SetEdgeWeight(T weight, int edgeIdx) = 0;
	
	virtual bool HasEdge(const MyVec2i& edges) const;
	virtual MyArray2i GetEdges() const;

	// guaranteed to be sorted from small to big index
	virtual MyArrayi GetEdgesByNodes(const MyArrayi& nodes) const;
	virtual MyArrayi GetNodesByEdges(const MyArrayi& edges) const;

	// return all neighbors of i
	// default implementations don't care about direction
	virtual MyArrayi GetNeighbors(int node) const;

	// for directed graph
	virtual MyArrayi GetNeighborsFrom(int from) const;
	virtual MyArrayi GetNeighborsIn(int to) const;
	
	// the default implementation does nothing about the following 4.
	virtual void AddEdge(const MyVec2i& edge, T weight = 1);
	virtual void DeleteEdge(int edgeIdx);
	virtual void AddNode(T weight = 1);
	virtual void DeleteNode(int nodeIdx);

	// naively use the above four to implement
	// to do low efficient implementation
	// reimplement to improve efficiency
	virtual void AddEdges(const MyArray2i& edges, T weight = 1);
	virtual void DeleteEdges(const MyArrayi& edges);
	virtual void AddNodes(int nNodes, T weight = 1);
	virtual void DeleteNodes(const MyArrayi& nodes);

	// does nothing
	virtual void LeaveOnlyNodes(MyArrayi nodes);
	virtual void LeaveOnlyEdges(MyArrayi edges);
};


template<typename T>
MyVec2i MyGenericGraph<T>::DecodeEdge(int edgeName){
	MyVec2i rst;
	rst[0] = edgeName&GRAPH_EDGE_NAME_MASK;
	rst[1] = (edgeName&(~GRAPH_EDGE_NAME_MASK)) >> 16;
	return rst;
}
	
template<typename T>
int MyGenericGraph<T>::EncodeEdge(MyVec2i edge){
	int rst = edge[0] + (edge[1] << 16);
	return rst;
}

template<typename T>
MyGenericGraph<T>::MyGenericGraph(const MyGenericGraph<T>& graph){
	MyGenericGraph<T>::MyGenericGraph(graph->GetNumNodes());
	this->AddEdges(graph->GetEdges());
	for(int i = 0;i<graph->GetNumNodes();i++){
		this->SetNodeWeight(graph->GetNodeWeight(i),i);
	}
	for(int i = 0;i<graph->GetNumEdges();i++){
		this->SetEdgeWeight(graph->GetEdgeWeight(i),i);
	}
}

template<typename T>
bool MyGenericGraph<T>::HasEdge(const MyVec2i& nodes) const{
	int edge = this->GetEdgeIndex(nodes);
	if(edge >= 0){
		return true;
	}
	return false;
}
  
template<typename T>
MyArray2i MyGenericGraph<T>::GetEdges() const{
	MyArray2i edges;
	for(int i = 0;i<this->GetNumEdges();i++){
		edges.push_back(this->GetEdge(i));
	}
	return edges;
}

template<typename T>
MyArrayi MyGenericGraph<T>::GetEdgesByNodes(const MyArrayi& nodes) const{
	MyArrayi edges;
	std::vector<bool> edgesIn(this->GetNumEdges(),false);
	for(int i = 0;i<nodes.size();i++){
		for(int j = 0;j<nodes.size();j++){
			if(this->HasEdge(MyVec2i(nodes[i],nodes[j]))){
				int edgeIdx = this->GetEdgeIndex(MyVec2i(i,j));
				edgesIn[edgeIdx] = true;
			}
		}
	}
	for(int i = 0;i<edgesIn.size();i++){
		if(edgesIn[i]){
			edges.push_back(i);
		}
	}
	return edges;
}
	
template<typename T>
MyArrayi MyGenericGraph<T>::GetNodesByEdges(const MyArrayi& edges) const{
	MyArrayi nodes;
	std::vector<bool> nodesIn(this->GetNumNodes(),false);
	for(int i = 0;i<edges.size();i++){
		MyVec2i nodes = this->GetEdge(edges[i]);
		nodesIn[nodes[0]] = true;
		nodesIn[nodes[1]] = true;
	}
	for(int i = 0;i<nodesIn.size();i++){
		if(nodesIn[i]){
			nodes.push_back(i);
		}
	}
	return nodes;
}

template<typename T>
MyArrayi MyGenericGraph<T>::GetNeighbors(int node) const{
	MyArrayi neighbours;
	for(int i = 0;i<this->GetNumNodes();i++){
		if(this->HasEdge(MyVec2i(node,i))
			|| this->HasEdge(MyVec2i(i,node))){
			neighbours.push_back(i);
		}
	}
	return neighbours;
};
	
template<typename T>
MyArrayi MyGenericGraph<T>::GetNeighborsFrom(int from) const{
	MyArrayi neighbours;
	for(int i = 0;i<this->GetNumNodes();i++){
		if(this->HasEdge(MyVec2i(from,i))){
			neighbours.push_back(i);
		}
	}
	return neighbours;
};
	
template<typename T>
MyArrayi MyGenericGraph<T>::GetNeighborsIn(int to) const{
	MyArrayi neighbours;
	for(int i = 0;i<this->GetNumNodes();i++){
		if(this->HasEdge(MyVec2i(i,to))){
			neighbours.push_back(i);
		}
	}
	return neighbours;
};

template<typename T>
void MyGenericGraph<T>::AddEdge(const MyVec2i& edges, T weight = 1){
}
	
template<typename T>
void MyGenericGraph<T>::DeleteEdge(int edgeIdx){
}
	
template<typename T>
void MyGenericGraph<T>::AddNode(T weight){
}
	
template<typename T>
void MyGenericGraph<T>::DeleteNode(int nodeIdx){
}
	
template<typename T>
void MyGenericGraph<T>::AddEdges(const MyArray2i& edges, T weight){
	for(int i = 0;i<edges.size();i++){
		this->AddEdge(edges[i],weight);
	}
}
	
template<typename T>
void MyGenericGraph<T>::DeleteEdges(const MyArrayi& edges){
	for(int i = 0;i<edges.size();i++){
		this->DeleteEdge(edges[i]);
	}
}
	
template<typename T>
void MyGenericGraph<T>::AddNodes(int nNodes, T weight){
	for(int i = 0;i<nNodes;i++){
		this->AddNode(weight);
	}
}
	
template<typename T>
void MyGenericGraph<T>::DeleteNodes(const MyArrayi& nodes){
	for(int i = 0;i<nodes.size();i++){
		this->DeleteNode(nodes[i]);
	}
}

template<typename T>
void MyGenericGraph<T>::LeaveOnlyNodes(MyArrayi nodes){
}
	
template<typename T>
void MyGenericGraph<T>::LeaveOnlyEdges(MyArrayi edges){
}

typedef MyGenericGraph<float> MyGenericGraphf;
typedef MyGenericGraph<bool> MyGenericGraphb;
typedef MyGenericGraph<int> MyGenericGraphi;