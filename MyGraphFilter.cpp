#include "MyGraphFilter.h"
#include "MyDenseGraph.h"
#include "MyGenericHierarchyOverlay.h"

#include <algorithm>

MyGraphFilter::MyGraphFilter(void)
{
}


MyGraphFilter::~MyGraphFilter(void)
{
}

void MyGraphFilter::SetInputGraph(const MyGenericGraphf* graph){
	mInGraph = graph;
}

MyGenericGraphf* MyGraphFilter::MakeFilteringByEdgeWeight(float minWeight, float maxWeight){
	this->GetNodesByEdgeWeight(mNewToOldNodeIndexMap,minWeight,maxWeight);
	//int n = mNewToOldNodeIndexMap.size();
	MyDenseGraphf* graph = new MyDenseGraphf;
	graph->Copy(*mInGraph);

	//std::vector<int> edges;
	//this->GetEdgesByEdgeWeight(edges,minWeight,maxWeight);
	//graph->LeaveOnlyEdges(edges);

	for(int i = 0;i<graph->GetNumEdges();i++){
		if(graph->GetEdgeWeight(i)<minWeight || graph->GetEdgeWeight(i)>maxWeight){
			graph->DeleteEdge(i);
			i--;
		}
	}
	return graph;
}


MyGenericGraphf* MyGraphFilter::MakeFilteringByEdgeWeightRatio(float ratioleft){
	int numEdges = mInGraph->GetNumEdges();
	MyArrayf weights(numEdges);
	for(int i = 0;i<numEdges;i++){
		weights[i] = mInGraph->GetEdgeWeight(i);
	}
	std::sort(weights.begin(), weights.end());
	int idx = ceil((1-ratioleft)*numEdges);
	float thres = weights[idx];
	return this->MakeFilteringByEdgeWeight(thres, 9999999.f);
}

void MyGraphFilter::GetNodesByEdgeWeight(std::vector<int>& nodeIdx, float minWeight, float maxWeight) const{
	std::vector<bool> remainNodes(mInGraph->GetNumNodes(),false);
	for(int i = 0;i<mInGraph->GetNumEdges();i++){
		float weight = mInGraph->GetEdgeWeight(i);
		if(weight >= minWeight && weight <= maxWeight){
			MyVec2i edge = mInGraph->GetEdge(i);
			remainNodes[edge[0]] = true;
			remainNodes[edge[1]] = true;
		}
	}
	for(int i = 0;i<remainNodes.size();i++){
		if(remainNodes[i]){
			nodeIdx.push_back(i);
		}
	}
}

void MyGraphFilter::GetEdgesByEdgeWeight(std::vector<int>& edgeIdx, float minWeight, float maxWeight) const{
	std::vector<bool> remainEdges(mInGraph->GetNumEdges(),false);
	for(int i = 0;i<mInGraph->GetNumEdges();i++){
		float weight = mInGraph->GetEdgeWeight(i);
		if(weight >= minWeight && weight <= maxWeight){
			remainEdges[i] = true;
		}
	}
	for(int i = 0;i<remainEdges.size();i++){
		if(remainEdges[i]){
			edgeIdx.push_back(i);
		}
	}
}
const int MyGraphFilter::GetInputNodeIndex(int outputNodeIndex) const{
	return mNewToOldNodeIndexMap[outputNodeIndex];
}

const int MyGraphFilter::GetOutputNodeIndex(int inputNodeIndex) const{
	for(int i = 0;i<mNewToOldNodeIndexMap.size();i++){
		if(mNewToOldNodeIndexMap[i] == inputNodeIndex){
			return i;
		}
	}
	return -1;
}

MyArray<const MyGenericNode*> arr;
int depth;
void AddDepth(const MyGenericNode* node){
	int nodeDepth = node->GetDepth();
	if(nodeDepth == depth
		|| (!node->HasAnyChild() && nodeDepth<depth)){
		arr.push_back(node);
	}
}

MyArray<MyGenericGraphf*>* MyGraphFilter::MakeGraphsByHierarchy(const MyGenericNode* hierarchyRoot, MyArray<MyArray<MyArrayi>>* nodesLod) const{
	MyArray<MyGenericGraphf*>* rst = new MyArray<MyGenericGraphf*>;
	bool needDeleteNodeArr = false;
	if(!nodesLod){
			nodesLod = new MyArray<MyArray<MyArrayi>>;
			needDeleteNodeArr = true;
	}
	int nDepth = hierarchyRoot->GetHeight()+1;
	for(int iDepth = 0;iDepth<nDepth;iDepth++){
		// get nodes at current depth
		arr.clear();
		depth = iDepth;
		hierarchyRoot->TraverseDescendants(AddDepth);
		int numNode = arr.size();
		
		// this nodes array contains the node of each cluster
		// at current depth
		MyArray<MyArrayi> nodeArr;
		nodeArr.resize(numNode);
		for(int inode = 0;inode<nodeArr.size();inode++){
			MyArrayStr nodeStrArr;
			MyGenericHierarchyOverlayStr::AddLeaveObjectToArray(&nodeStrArr,arr[inode]);
			for(int i = 0;i<nodeStrArr.size();i++){
				nodeArr[inode].push_back(nodeStrArr[i].ToInt());
			}
		}
		MyDenseGraphf* graph = new MyDenseGraphf(numNode);

		// get each edge weigh for new graph
		int numItr = 0;
		for(int i = 0;i<numNode;i++){
			const MyArrayi& nodesi = nodeArr[i];
			for(int j = 0;j<numNode;j++){
				const MyArrayi& nodesj = nodeArr[j];
				float ijConnectStrength = 0.f;
				int numIjConnection = 0;
				if(i!=j){
					for(int ii = 0;ii<nodesi.size();ii++){
						for(int jj = 0;jj<nodesj.size();jj++){
							numItr++;
							MyVec2i edge(nodesi[ii],nodesj[jj]);
							if(mInGraph->HasEdge(edge)){
								numIjConnection ++;
								int edgeIdx = mInGraph->GetEdgeIndex(edge);
								ijConnectStrength += mInGraph->GetEdgeWeight(edgeIdx);
							}
						}					
					}
				}
				else{
					ijConnectStrength = 0.f;
					numIjConnection = 0;
				}
				// if any edge of the cluster is connected
				// the cluster is connected
				if(numIjConnection>0){
					MyVec2i edge(i,j);
					// the strength is the average strength
					//graph->AddEdge(edge,ijConnectStrength/numIjConnection);
					// the strength is the num connections
					graph->AddEdge(edge,numIjConnection);
				}
			}
		}
		rst->push_back(graph);
		nodesLod->push_back(nodeArr);
	}
	if(needDeleteNodeArr){
		delete nodesLod;
	}
	return rst;
}