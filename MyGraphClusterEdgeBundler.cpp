#include "MyGraphClusterEdgeBundler.h"
#include "MyArray.h"
#include "MyGenericHierarchyOverlay.h"
#include "MyGraphLayout.h"
#include <cassert>
#define MAXSTIFFNESS 9999999.f
#define EPS 0.00000001f

MyGraphClusterEdgeBundler::MyGraphClusterEdgeBundler(void)
{
	mNumControlPoints = 15;
	mStiffness = 0.1f;
	mUseLeastCommonAncestor = true;
}


MyGraphClusterEdgeBundler::~MyGraphClusterEdgeBundler(void)
{
}
	
void MyGraphClusterEdgeBundler::SetHierarchy(const MyGenericNode* hierarchy){
	mHierarchy = hierarchy;
}

void MyGraphClusterEdgeBundler::SetUseLeastCommomAncestor(bool use){
	mUseLeastCommonAncestor = use;
}
/*
void MyGraphClusterEdgeBundler::Update(){

	// group all nodes
	MyArray<MyArrayi> clusterSet;
	for(int i = 0;i<mHierarchy->GetNumChildren();i++){
		MyArrayStr nameArray;
		clusterSet.push_back(MyArrayi());
		MyGenericHierarchyOverlayStr::AddLeaveObjectToArray(&nameArray, mHierarchy->GetChild(i));
		for(int namei = 0;namei<nameArray.size();namei++){
			clusterSet[i].push_back(nameArray[namei].ToInt());
		}
	}
	
	// map each node to cluster in
	MyArrayi nodeToCluster(mInGraph->GetNumNodes(),-1);
	for(int iCluster = 0;iCluster<clusterSet.size();iCluster++){
		for(int iNode = 0;iNode<clusterSet[iCluster].size();iNode++){
			int node = clusterSet[iCluster][iNode];
			nodeToCluster[node] = iCluster;
		}
	}

	// get cluster center;
	MyArray3f clusterCenter;
	for(int iCluster = 0;iCluster<clusterSet.size();iCluster++){
		MyVec3f center(0.f,0.f,0.f);
		for(int iNode = 0;iNode<clusterSet[iCluster].size();iNode++){
			int node = clusterSet[iCluster][iNode];
			center += mPos->at(node);
		}
		if(clusterSet[iCluster].size()>0){
			center /= clusterSet[iCluster].size();
		}
		clusterCenter.push_back(center);
	}

	// group edge
	MyArray<MyArrayi> edgeGroup;
	MyArray2i edgeGroupName;
	for(int iEdge = 0;iEdge<mInGraph->GetNumEdges();iEdge++){
		MyVec2i edge = mInGraph->GetEdge(iEdge);
		int edgeACluster = nodeToCluster[edge[0]];
		int edgeBCluster = nodeToCluster[edge[1]];
		MyVec2i edgeClusterName(edgeACluster, edgeBCluster);
		int edgeGroupIdx = edgeGroupName.IndexOf(edgeClusterName);
		if(edgeGroupIdx >= 0){
			edgeGroup[edgeGroupIdx].push_back(iEdge);
		}
		else{
			edgeGroupName.push_back(edgeClusterName);
			edgeGroup.push_back(MyArrayi());
			edgeGroup.back().push_back(iEdge);
		}

	}

	// init edge segments
	MyArrayf stiffnesses(mInGraph->GetNumEdges());
	mControPoints.clear();
	for(int iEdge = 0;iEdge<mInGraph->GetNumEdges();iEdge++){
		mControPoints.push_back(MyArray3f(mNumControlPoints+2));
		MyVec2i edge = mInGraph->GetEdge(iEdge);
		MyVec3f startPos = mPos->at(edge[0]);
		MyVec3f EndPos = mPos->at(edge[1]);
		// define stiffness
		float lengthEdge = (startPos-EndPos).norm();
		//float stiffness = lengthEdge/mNumControlPoints;
		float stiffness;
		if(lengthEdge == 0){
			stiffness = MAXSTIFFNESS;
		}
		else{
			stiffness = mStiffness/(lengthEdge*mNumControlPoints);
			//stiffness = mStiffness/lengthEdge;
		}
		stiffnesses[iEdge] = stiffness;
		mControPoints[iEdge][0] = startPos;
		for(int iP = 0;iP<mNumControlPoints;iP++){
			float partial = (float)(iP+1)/(mNumControlPoints+1);
			MyVec3f interPoints = (1-partial)*startPos+partial*EndPos;
			mControPoints[iEdge][iP+1] = interPoints;
		}
		mControPoints[iEdge][mNumControlPoints+1] = EndPos;
	}

	// start bundling
	int numCycles = 5;
	float displacement = 0.4f;
	int iteration = 50;
	for(int iCycle = 0; iCycle < numCycles; iCycle++){

		// for each iteration
		for(int ii = 0;ii < iteration; ii++){
			// bkup the result
			MyArray<MyArray3f> oldControPoints = mControPoints;
			
			// for each edge group
			for(int iGroup=0;iGroup < edgeGroup.size();iGroup++){
				if(edgeGroup.size()<=1){
					continue;
				}
				// for each edge in the group
				for(int iEdgeA = 0;iEdgeA<edgeGroup[iGroup].size();iEdgeA ++){
					int iedge = edgeGroup[iGroup][iEdgeA];
					MyVec2i edge = mInGraph->GetEdge(iedge);
					if(edge[0] == edge[1]){
						continue;
					}
					// for each control point
					for(int iP = 1;iP<mNumControlPoints+1;iP++){
						MyVec3f pos = oldControPoints[iedge][iP];
						MyVec3f force(0.f,0.f,0.f);

						// force from previous control point in the same edge
						MyVec3f prevPos = oldControPoints[iedge][iP-1];
						force += prevPos-pos;
						// force from next control point in the same edge
						MyVec3f nextPos = oldControPoints[iedge][iP+1];
						force += nextPos -pos;
						// add stiffness here to save one multiplication
						force *= stiffnesses[iedge];

						
						
						//// force from all other same-index control points
						//// from edge in the same group
						//for(int iEdgeB = 0;iEdgeB<edgeGroup[iGroup].size();iEdgeB ++){
						//	if(iEdgeB != iEdgeA){
						//		int iedgeB = edgeGroup[iGroup][iEdgeB];
						//		MyVec2i edgeB = mInGraph->GetEdge(iedgeB);
						//		if(edgeB[0] == edgeB[1] ||(edgeB[0] == edge[1] && edgeB[1] == edge[0])){
						//			continue;
						//		}

						//		// reverse direction, skip
						//		if((oldControPoints[iedgeB][0]-oldControPoints[iedgeB][mNumControlPoints+1])
						//			*(oldControPoints[iedge][0]-oldControPoints[iedge][mNumControlPoints+1])<=0){
						//			continue;
						//		}
						//		MyVec3f posB = oldControPoints[iedgeB][iP];
						//		MyVec3f dir = posB-pos;
						//		float distance = dir.norm();
						//		if(distance > EPS){
						//			force += dir/(distance*distance);
						//		}
						//	}
						//}
						
						
						// use the center point of the between cluster spine edge
						int clusterA = nodeToCluster[edge[0]];
						int clusterB = nodeToCluster[edge[1]];
						if(clusterA != clusterB){
							float idxf = 0.33333;//(iP+1)/(mNumControlPoints+1);
							float midxf = 1.f-idxf;
							MyVec3f center1 = clusterCenter[clusterA]*idxf
								+clusterCenter[clusterB]*midxf;
							MyVec3f center2 = clusterCenter[clusterA]*midxf
								+clusterCenter[clusterB]*idxf;
							MyVec3f toCenter1 = center1 - pos;
							MyVec3f toCenter2 = center2 - pos;
							MyVec3f toCenterForce(0.f,0.f,0.f);
							float strengthCluster = edgeGroup[iGroup].size();
							float distance1 = toCenter1.norm();
							float distance2 = toCenter2.norm();
							if(distance1 <= distance2){
								if(distance1 > EPS){
									toCenterForce = toCenter1*stiffnesses[iedge]*strengthCluster;
								}
							}
							else{
								if(distance2 > EPS){
									toCenterForce = toCenter2*stiffnesses[iedge]*strengthCluster;
								}
							}

							// strength modified by number of edges
							force += toCenterForce;
						}

						
						//// repell by other groups
						//// now it destroys everthing!!!!!!!!!!!!!!!!
						//for(int iOtherClusterA = 0;iOtherClusterA< clusterSet.size();iOtherClusterA++){
						//	for(int iOtherClusterB = 0;iOtherClusterB< clusterSet.size();iOtherClusterB++){
						//		if(!((iOtherClusterA == nodeToCluster[edge[0]] && iOtherClusterB == nodeToCluster[edge[1]])
						//			||(iOtherClusterA == nodeToCluster[edge[1]] && iOtherClusterB == nodeToCluster[edge[0]]))){
						//			MyVec3f otherCenter = clusterCenter[iOtherClusterA]*0.5f
						//					+clusterCenter[iOtherClusterA]*0.5f;
						//			MyVec3f toOtherCenter = otherCenter-pos;
						//			float toOtherCenterDistance = toOtherCenter.norm();
						//			float hookLength = toOtherCenter.norm()-5;
						//			hookLength = (hookLength<0?0:hookLength);
						//			MyVec3f repellForce = -hookLength*toOtherCenter/toOtherCenterDistance;
						//			force += repellForce;
						//		}
						//	}
						//}
						

						// to sum up
						mControPoints[iedge][iP] += force*displacement;
					} // end point
				} // end edge
			} // end group
		} // end iteration
		displacement /= 2;
		iteration *= 0.667;
	} // end cycle
}
*/

void MyGraphClusterEdgeBundler::Update(){
	// group all nodes
	MyArray<MyArrayi> clusterSet;
	for(int i = 0;i<mHierarchy->GetNumChildren();i++){
		MyArrayStr nameArray;
		clusterSet.push_back(MyArrayi());
		MyGenericHierarchyOverlayStr::AddLeaveObjectToArray(&nameArray, mHierarchy->GetChild(i));
		for(int namei = 0;namei<nameArray.size();namei++){
			clusterSet[i].push_back(nameArray[namei].ToInt());
		}
	}
	
	// map each node to cluster in
	MyArrayi nodeToCluster(mLayout->GetGraph()->GetNumNodes(),-1);
	for(int iCluster = 0;iCluster<clusterSet.size();iCluster++){
		for(int iNode = 0;iNode<clusterSet[iCluster].size();iNode++){
			int node = clusterSet[iCluster][iNode];
			nodeToCluster[node] = iCluster;
		}
	}

	// get cluster center;
	MyArray3f* clusterCenterPtr = mLayout->MakeCLusterCenterArray(clusterSet);
	MyArray3f clusterCenter = *clusterCenterPtr;
	delete clusterCenterPtr;
	/*
	for(int iCluster = 0;iCluster<clusterSet.size();iCluster++){
		MyVec3f center(0.f,0.f,0.f);
		for(int iNode = 0;iNode<clusterSet[iCluster].size();iNode++){
			int node = clusterSet[iCluster][iNode];
			center += mPos->at(node);
		}
		if(clusterSet[iCluster].size()>0){
			center /= clusterSet[iCluster].size();
		}
		clusterCenter.push_back(center);
	}
	*/

	// group edge
	MyArray<MyArrayi> edgeGroup;
	MyArray2i edgeGroupName;
	for(int iEdge = 0;iEdge<mLayout->GetGraph()->GetNumEdges();iEdge++){
		MyVec2i edge = mLayout->GetGraph()->GetEdge(iEdge);
		int edgeACluster = nodeToCluster[edge[0]];
		int edgeBCluster = nodeToCluster[edge[1]];
		MyVec2i edgeClusterName(edgeACluster, edgeBCluster);
		int edgeGroupIdx = edgeGroupName.IndexOf(edgeClusterName);
		if(edgeGroupIdx >= 0){
			edgeGroup[edgeGroupIdx].push_back(iEdge);
		}
		else{
			edgeGroupName.push_back(edgeClusterName);
			edgeGroup.push_back(MyArrayi());
			edgeGroup.back().push_back(iEdge);
		}

	}

	// init edge segments
	MyArrayf stiffnesses(mLayout->GetGraph()->GetNumEdges());
	mControPoints.clear();
	int nExtP = 2;
	if(mUseLeastCommonAncestor){
		nExtP += 1;
	}
	for(int iEdge = 0;iEdge<mLayout->GetGraph()->GetNumEdges();iEdge++){
		mControPoints.push_back(MyArray3f(nExtP+2));
		MyVec2i edge = mLayout->GetGraph()->GetEdge(iEdge);
		MyVec3f startPos = mLayout->GetNodePos(edge[0]);
		MyVec3f EndPos = mLayout->GetNodePos(edge[1]);
		// define stiffness
		float lengthEdge = (startPos-EndPos).norm();
		//float stiffness = lengthEdge/2;
		float stiffness;
		if(lengthEdge == 0){
			stiffness = MAXSTIFFNESS;
		}
		else{
			stiffness = mStiffness/(lengthEdge*2);
			//stiffness = mStiffness/lengthEdge;
		}
		stiffnesses[iEdge] = stiffness;
		mControPoints[iEdge][0] = startPos;
		for(int iP = 0;iP<nExtP;iP++){
			float partial = (float)(iP+1)/(nExtP+1);
			MyVec3f interPoints = (1-partial)*startPos+partial*EndPos;
			mControPoints[iEdge][iP+1] = interPoints;
		}
		mControPoints[iEdge][nExtP+1] = EndPos;
	}

	// start bundling
	MyArray<MyArray3f> oldControPoints = mControPoints;
	float strength = 1.f;
	// for each edge group
	for(int iGroup=0;iGroup < edgeGroup.size();iGroup++){
		if(edgeGroup.size()<=1){
			continue;
		}
		// for each edge in the group
		for(int iEdgeA = 0;iEdgeA<edgeGroup[iGroup].size();iEdgeA ++){
			int iedge = edgeGroup[iGroup][iEdgeA];
			MyVec2i edge = mLayout->GetGraph()->GetEdge(iedge);
			if(edge[0] == edge[1]){
				continue;
			}
			
			int clusterA = nodeToCluster[edge[0]];
			int clusterB = nodeToCluster[edge[1]];
			if(clusterA == clusterB){
				if(mUseLeastCommonAncestor){
					MyVec3f c = clusterCenter[clusterA];
					MyArray3f cpts;
					cpts.push_back(mControPoints[iedge].front());
					cpts.push_back(c);
					cpts.push_back(mControPoints[iedge].back());
					mControPoints[iedge] = cpts;
				}
				continue;
			}


			if(mUseLeastCommonAncestor){
				MyVec3f p1 = mControPoints[iedge][1];
				MyVec3f p2 = mControPoints[iedge][2];
				MyVec3f p3 = mControPoints[iedge][3];
				MyVec3f c1 = clusterCenter[clusterA];
				// the root is centered
				MyVec3f c2(0,0,0);
				MyVec3f c3 = clusterCenter[clusterB];

				mControPoints[iedge][1] = p1*(1-strength) + c1*strength;
				mControPoints[iedge][2] = p2*(1-strength) + c2*strength;
				mControPoints[iedge][3] = p3*(1-strength) + c3*strength;

				assert(mControPoints[iedge].size() == 5);
				
			}
			else{
				MyVec3f p1 = mControPoints[iedge][1];
				MyVec3f p2 = mControPoints[iedge][2];
				MyVec3f c1 = clusterCenter[clusterA];
				MyVec3f c2 = clusterCenter[clusterB];
				mControPoints[iedge][1] = p1*(1-strength) + c1*strength;
				mControPoints[iedge][2] = p2*(1-strength) + c2*strength;

				assert(mControPoints[iedge].size() == 4);
			}

		} // end edge
	} // end group

	this->SmoothControlPoints(mNumControlPoints+2,0.5);
}

#undef MAXSTIFFNESS
#undef EPS