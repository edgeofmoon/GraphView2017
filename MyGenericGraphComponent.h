#include "MyGenericGraph.h"
#include "MyArray.h"
#include "MyGenericHierarchyOverlay.h"
#include "MyUtility.h"

template<typename T>
class MyGenericGraphComponent
	: public MyGenericGraph<T>
{
public:
	MyGenericGraphComponent();
	~MyGenericGraphComponent();

	virtual void Copy(const MyGenericGraph<T>& other);

	// depth copy function
	//virtual MyGenericGraph<T>& operator=(const MyGenericGraph<T>& graph);

	// if no edge between nodes exists, return -1;
	virtual int GetEdgeIndex(const MyVec2i& edge) const;
	virtual MyVec2i GetEdge(int index) const;
	virtual int GetNode(int index) const;
	virtual const MyArray2i* GetEdges() const;
	virtual const MyArrayi* GetNodes() const;

	// all rest code index edge with i only
	virtual int GetNumEdges() const;
	virtual int GetNumNodes() const;
	
	// do nothing
	virtual const T GetEdgeWeight(int index) const;
	virtual const T GetNodeWeight(int index) const;
	virtual void SetNodeWeight(T weight, int nodeIdx);
	virtual void SetEdgeWeight(T weight, int edgeIdx);
	
	virtual void AddEdge(const MyVec2i& edge, T weight = 1);
	virtual void DeleteEdge(int edgeIdx);
	virtual void AddNode(T weight = 1);
	virtual void DeleteNode(int nodeIdx);
	
	static MyGenericSelectableDataAbstract* MakeTranslatedSelection(const MyGenericNode* selection);
	static MyGenericNode* MakeGenericSelection(const MyGenericSelectableDataAbstract* sub);

protected:
	MyArrayi* mNodes;
	MyArray2i* mEdges;

	void translateSelection(MyGenericGraphComponent<T>* target, const MyGenericNode* selection);
};

template<typename T>
MyGenericGraphComponent<T>::MyGenericGraphComponent(){
	mNodes = new MyArrayi;
	mEdges = new MyArray2i;
}
	
template<typename T>
MyGenericGraphComponent<T>::~MyGenericGraphComponent(){
	SafeFreeObject(mNodes);
	SafeFreeObject(mEdges);
}

template<typename T>
void MyGenericGraphComponent<T>::Copy(const MyGenericGraph<T>& other){
	const MyGenericGraphComponent<T>* pother
		= dynamic_cast<const MyGenericGraphComponent<T>*>(&other);
	if(other){
		mNodes->Copy(pother->mNodes);
		mEdges->Copy(pother->mEdges);
	}
}

template<typename T>
int MyGenericGraphComponent<T>::GetEdgeIndex(const MyVec2i& edge) const{
	return mEdges->IndexOf(edge);
}

template<typename T>
MyVec2i MyGenericGraphComponent<T>::GetEdge(int index) const{
	return mEdges->at(index);
}

template<typename T>
int MyGenericGraphComponent<T>::GetNode(int index) const{
	return mNodes->at(index);
}

template<typename T>
const MyArray2i* MyGenericGraphComponent<T>::GetEdges() const{
	return mEdges;
}
		
template<typename T>
const MyArrayi* MyGenericGraphComponent<T>::GetNodes() const{
	return mNodes;
}

template<typename T>
int MyGenericGraphComponent<T>::GetNumEdges() const{
	return mEdges->size();
}

template<typename T>
int MyGenericGraphComponent<T>::GetNumNodes() const{
	return mNodes->size();
}

template<typename T>
const T MyGenericGraphComponent<T>::GetEdgeWeight(int index) const{
	return -1;
}

template<typename T>
const T MyGenericGraphComponent<T>::GetNodeWeight(int index) const{
	return -1;
}

template<typename T>
void MyGenericGraphComponent<T>::SetNodeWeight(T weight, int nodeIdx){
}

template<typename T>
void MyGenericGraphComponent<T>::SetEdgeWeight(T weight, int edgeIdx){
}

template<typename T>
void MyGenericGraphComponent<T>::AddEdge(const MyVec2i& edge, T weight){
	mEdges->PushBackUniqueOne(edge);
}

template<typename T>
void MyGenericGraphComponent<T>::DeleteEdge(int edgeIdx){
}
template<typename T>
void MyGenericGraphComponent<T>::AddNode(T weight){
}
template<typename T>
void MyGenericGraphComponent<T>::DeleteNode(int nodeIdx){
}

template<typename T>
MyGenericSelectableDataAbstract* MyGenericGraphComponent<T>::MakeTranslatedSelection(const MyGenericNode* selection){
	MyGenericGraphComponent<T>* graphComp = new MyGenericGraphComponent<T>;
	this->translateSelection(graphComp, selection);
	return graphComp;
}

template<typename T>
MyGenericNode* MyGenericGraphComponent<T>::MakeGenericSelection(const MyGenericSelectableDataAbstract* sub){
	const MyGenericGraphComponent<T>* graphComp = dynamic_cast<const MyGenericGraphComponent<T>*> sub;
	if(graphComp){
		MyGenericHierarchyOverlayStr* root = new MyGenericHierarchyOverlayStr("Selection");
		MyGenericHierarchyOverlayi* nodes = new MyGenericHierarchyOverlayi(MyGenericGraph::GRAPH_NODE);
		for(int i = 0;i<graphComp->GetNumNodes();i++){
			MyGenericHierarchyOverlayi* nodei = new MyGenericHierarchyOverlayi(graphComp->GetNode(i));
			nodes->AddChild(nodei);
		}
		root->AddChild(nodes);
		MyGenericHierarchyOverlayi* edges = new MyGenericHierarchyOverlayi(MyGenericGraph::GRAPH_EDGE);
		for(int i = 0;i<graphComp->GetNumEdges();i++){
			MyGenericHierarchyOverlayi* edgei = new MyGenericHierarchyOverlayi(graphComp->GetEdge(i));
			nodes->AddChild(edgei);
		}
		root->AddChild(edges);
		return root;
	}
	else{
		return 0;
	}
}

template<typename T>
void MyGenericGraphComponent<T>::translateSelection(MyGenericGraphComponent<T>* target, const MyGenericNode* selection){
	const MyGenericHierarchyOverlayi* node = dynamic_cast<const MyGenericHierarchyOverlayi*>(selection);
	if(node){
		if (node->GetObject() == MyGenericGraph::GRAPH_NODE){
			for(int i = 0;i<node->GetNumChildren();i++){
				const MyGenericHierarchyOverlayi* child
					= dynamic_cast<const MyGenericHierarchyOverlayi*>(selection->GetChild(i));
				target->AddNode(child->GetObject());
			}
		}
		if (node->GetObject() == MyGenericGraph::GRAPH_EDGE){
			for(int i = 0;i<node->GetNumChildren();i++){
				const MyGenericHierarchyOverlay2i* child
					= dynamic_cast<const MyGenericHierarchyOverlay2i*>(selection->GetChild(i));
				target->AddEdge(child->GetObject());
			}
		}
	}
	else{
		for (int i = 0; i < node->GetNumChildren(); i++){
			this->translateSelection(target,selection->GetChild(i));
		}
	}
}