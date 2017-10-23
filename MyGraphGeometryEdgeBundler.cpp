#include "MyGraphGeometryEdgeBundler.h"
#include "MySimple2DGrid.h"
#include "MyBoundingBox.h"
#include "MyPolyLine.h"
#include "MyGraphDrawer.h"

#include "myMesh.h"
#include "MyGraphLayout.h"
//#include <iostream>

MyGraphGeometryEdgeBundler::MyGraphGeometryEdgeBundler(int dx, int dy)
{
	mDivisionX = dx;
	mDivisionY = dy;
}


MyGraphGeometryEdgeBundler::~MyGraphGeometryEdgeBundler(void)
{
}

void MyGraphGeometryEdgeBundler::Update(){
	grid.SetXDivision(mDivisionX);
	grid.SetYDivision(mDivisionY);
	const MyGenericGraphf* graph = mLayout->GetGraph();
	MyBoundingBox box;
	for(int i = 0;i<graph->GetNumNodes();i++){
		MyVec3f pos = mLayout->GetNodePos(i);
		box.Engulf(pos);
	}
	MyVec2f lowPos(box.GetLowPos()[0], box.GetLowPos()[1]);
	MyVec2f highPos(box.GetHighPos()[0], box.GetHighPos()[1]);
	grid.SetRange(lowPos, highPos);

	MyArray<MyArray<MyArray<MyVec2i>>> edgeContained(mDivisionX,mDivisionY);
	//edgeContained.resize(mDivisionX);
	//for(int i = 0;i<mDivisionX;i++){
	//	edgeContained[i].resize(mDivisionY);
	//}

	for(int i = 0;i<graph->GetNumEdges();i++){
		MyVec2i edge = graph->GetEdge(i);

		MyVec3f start3f = mLayout->GetNodePos(edge[0]);
		MyVec3f end3f = mLayout->GetNodePos(edge[1]);

		MyVec2f start(start3f[0], start3f[1]);
		MyVec2f end(end3f[0], end3f[1]);

		MyArray2i* passedCells = grid.MakePassGridIndexArray(start, end);
		
		for(int ic = 0;ic<passedCells->size();ic++){

			// tmp add
			if (passedCells->at(ic)[0] >= mDivisionX || passedCells->at(ic)[1] >= mDivisionY){
				continue;
			}
			// end tmp add
			edgeContained[passedCells->at(ic)[0]][passedCells->at(ic)[1]].push_back(edge);
		}

		delete passedCells;
	}

	float stdAngleThre = 1.f;
	mainAngles = MyArray<MyArrayf>(mDivisionX,mDivisionY);
	for(int x = 0;x<edgeContained.size();x++){
		for(int y = 0;y<edgeContained[x].size();y++){
			float sumAngle = 0.f;
			MyArrayf angles;
			for(int i = 0;i<edgeContained[x][y].size();i++){
				MyVec3f start = mLayout->GetNodePos(edgeContained[x][y][i][0]);
				MyVec3f end = mLayout->GetNodePos(edgeContained[x][y][i][1]);
				float angle = atan2(end[1]-start[1], end[0]-start[0]);
				if(angle<0) angle += 3.141592658;
				angles.push_back(angle);
				sumAngle += angle;
			}
			float meanAngle;
			float stdAngle;
			if(angles.size() <=0){
				meanAngle = -1.f;
				stdAngle = -1.f;
			}
			else{
				//meanAngle = sumAngle / angles.size();
				float cosSum = 0.f;
				float sinSum = 0.f;
				for(int i = 0;i<angles.size();i++){
					cosSum += cos(angles[i]);
					sinSum += sin(angles[i]);
				}
				meanAngle = atan2(sinSum/angles.size(), cosSum/angles.size());
				float sumDivAngle = 0.f;
				for(int i = 0;i<angles.size();i++){
					float diff = angles[i]-meanAngle;
					//if(diff>3.141592658/2) diff -= 3.141592658/2;
					//if(diff<0) diff += 3.141592658/2;
					float div = diff*diff;
					sumDivAngle += div;
				}
				stdAngle = sqrtf(sumDivAngle/angles.size());
			}
			mainAngles[x][y] = meanAngle;
			if(stdAngle>stdAngleThre){
				mainAngles[x][y] = -1;
			}
		}
	}

	float angleThre = 20*3.14159/180;
	bool hasSomeMerge = true;
	//MyArray<MyArray2i> mergedGrids;
	//MyArrayf gridMainAngles;
	//MyArrayi gridWeight; // num edges

	// init merge
	// each cell is a merged grids
	for(int x = 0;x<edgeContained.size();x++){
		for(int y = 0;y<edgeContained[x].size();y++){
			mergedGrids.push_back(MyArray2i(1,MyVec2i(x,y)));
			gridMainAngles.push_back(mainAngles[x][y]);
			gridWeight.push_back(1);
		}
	}

	while(hasSomeMerge){
		hasSomeMerge = false;
		for(int i = 0;i<mergedGrids.size();i++){
			for(int j = i+1; j<mergedGrids.size();j++){
				if(fabs(gridMainAngles[i]-gridMainAngles[j])<angleThre
					&& MySimple2DGrid::IsCellConnected(mergedGrids[i], mergedGrids[j])){

					gridMainAngles[i] = (gridMainAngles[i]*gridWeight[i]+gridMainAngles[j]*gridWeight[j])/(gridWeight[i]+gridWeight[j]);
					gridWeight[i] += gridWeight[j];
					for(int newCellIdx = 0;newCellIdx<mergedGrids[j].size();newCellIdx++){
						mergedGrids[i].push_back(mergedGrids[j][newCellIdx]);
					}
					hasSomeMerge = true;

					mergedGrids.EraseAt(j);
					gridMainAngles.EraseAt(j);
					gridWeight.EraseAt(j);
					j--;
				}
			}
		}
	}

	for(int i = 0;i<mergedGrids.size();i++){
		if(gridMainAngles[i]<0.f) {
			gridLine.push_back(MyLine2f(MyVec2f(0,0),MyVec2f(0,0)));
			continue;
		};
		MyArray2f* contour = grid.MakeContourArray(mergedGrids[i]);
		int n = contour->size();
		MyBoundingBox box(contour->front().toDim<3>(0.f),0,0);
		for(int j = 1;j<n;j++){
			box.Engulf(contour->at(j).toDim<3>(0.f));
		}
		MyVec2f center = box.GetCenter().toDim<2>();
		float a = tan(gridMainAngles[i]+MY_PI/2);
		float b = center[1] - a*center[0];
		float leftX = box.GetLeft()-grid.GetCellWidth();
		float leftY = a*leftX+b;
		MyVec2f start( leftX, leftY);
		float rightX = box.GetRight()+grid.GetCellWidth();
		float right = a*rightX+b;
		MyVec2f end( rightX, right);

		MyPolyline2f polyline;
		polyline.SetLine(*contour);
		polyline.SetLoop(true);

		MyArray<MyLine2f>* lines = polyline.MakeClippedLineArray(MyLine2f(start, end));

		float maxlength = -0.1f;
		int maxIdx;
		for(int j = 0;j<lines->size();j++){
			float length = lines->at(j).GetLength();
			if(length>maxlength){
				maxlength = length;
				maxIdx = j;
			}
		}
		gridLine.push_back(lines->at(maxIdx));

		// make sure no line ends share the same x
		for(int i = 0;i<gridLine.size();i++){
			bool sameX = true;
			// for start
			do{
				MyVec2f lstart= gridLine[i].GetStart();
				sameX = false;
				for(int j = 0;j<gridLine.size();j++){
					if(i==j) continue;
					if(lstart[0] == gridLine[j].GetStart()[0] || lstart[0] == gridLine[j].GetEnd()[0]){
						gridLine[i].SetStart(lstart+MyVec2f(0.0005f,0));
						sameX = true;
					}
				}
				if(lstart[0] == gridLine[i].GetEnd()[0]){
					gridLine[i].SetStart(lstart+MyVec2f(0.0005f,0));
					sameX = true;
				}
			}while(sameX);

			// for end
			do{
				MyVec2f lend= gridLine[i].GetEnd();
				sameX = false;
				for(int j = 0;j<gridLine.size();j++){
					if(i==j) continue;
					if(lend[0] == gridLine[j].GetStart()[0] || lend[0] == gridLine[j].GetEnd()[0]){
						gridLine[i].SetEnd(lend+MyVec2f(0.0005f,0));
						sameX = true;
					}
				}
				if(lend[0] == gridLine[i].GetStart()[0]){
					gridLine[i].SetEnd(lend+MyVec2f(0.0005f,0));
					sameX = true;
				}
			}while(sameX);
		}

		delete lines;
		delete contour;
	}
	
	MyArray<MyLine2f> nonCrossedLines;
	for(int i = 0;i<gridLine.size();i++){
		if(gridMainAngles[i]<0) continue;
		bool crossed = false;
		for(int j = 0;j<nonCrossedLines.size();j++){
			if(MyLine2f::Intersected(gridLine[i],nonCrossedLines[j])){
				crossed = true;
				break;
			}
		}
		if(!crossed){
			nonCrossedLines.push_back(gridLine[i]);
		}
	}
	gridLine = nonCrossedLines;
	MyArray<MyVec2f> points;
	for(int i = 0;i<nonCrossedLines.size();i++){
		points.push_back(nonCrossedLines[i].GetStart());
		points.push_back(nonCrossedLines[i].GetEnd());
	}
	/*
	//MyArray<EPointer> meshEdge;
	MyArray<MyVec2f> points;
	for(int i = 0;i<gridLine.size();i++){
		if(gridMainAngles[i]<0) continue;
		points.push_back(gridLine[i].GetStart());
		points.push_back(gridLine[i].GetEnd());
	}
	*/
	// gen mesh
	myMesh mesh;
	mesh.loadPoints(points);
	mesh.buildCDT();
	//mesh.tagExternalAndDeleteOutsideEdges(points);
	//mesh.display();
	VPointer   v1,v2;
	EListPointer   el;
	el=mesh.e_head;
	while(el!=NULL){
		if   (el->e->type!=VIRTUAL){
			v1=el->e->he[1]->v;
			v2=el->e->he[0]->v;
			MyVec2f p1(v1->pos.x,v1->pos.y);
			MyVec2f p2(v2->pos.x,v2->pos.y);
			meshLines.push_back(MyLine2f(p1,p2));
			//meshEdge.push_back(el->e);
		}
		el=el->next;
	}

	// from mesh to control points
	MyArray<MyArrayi> edgeMeshInts(graph->GetNumEdges());
	for(int i = 0;i<meshLines.size();i++){
		MyVec2f intersection(0.f,0.f);
		int n = 0;
		for(int j = 0;j<graph->GetNumEdges();j++){
			MyVec2i edge = graph->GetEdge(j);
			MyLine2f edgeLine(mLayout->GetNodePos(edge[0]).toDim<2>(), mLayout->GetNodePos(edge[1]).toDim<2>());
			if(meshLines[i].Intersected(edgeLine)){
				intersection += meshLines[i].GetIntersection(edgeLine);
				n ++;
				edgeMeshInts[j] << i;
			}
		}
		if(n>0){
			intersections.push_back(intersection/n);
		}
		else{
			intersections.push_back(MyVec2f(-1,-1));
		}
	}
	// now route the edge to the control points
	mControPoints.resize(graph->GetNumEdges(),MyArray3f());
	
	for(int i = 0;i<graph->GetNumEdges();i++){
		MyVec2i edge = graph->GetEdge(i);
		MyLine2f edgeLine(mLayout->GetNodePos(edge[0]).toDim<2>(), mLayout->GetNodePos(edge[1]).toDim<2>());
		MyVec2f dir = edgeLine.GetUnitVector();

		mControPoints[i] << edgeLine.GetStart().toDim<3>(0);
		if(edgeMeshInts[i].size() >= 0){
			MyArrayf Ts;
			MyArray2f inters;
			for(int j = 0;j<edgeMeshInts[i].size();j++){
				MyVec2f intsec = intersections[edgeMeshInts[i][j]];
				float t = (intsec-edgeLine.GetStart())*dir;
				Ts << t;
				inters << intsec;
			}
			MyArrayi* sortRst = Ts.MakeSortResultArray();
			for(int j = 0;j<Ts.size();j++){
				mControPoints[i] << inters[sortRst->at(j)].toDim<3>(0);
			}
			delete sortRst;
		}
		mControPoints[i] << edgeLine.GetEnd().toDim<3>(0);
	}
	
	// find the problematic edges
	float angleWeight = 10.f;
	float distanceWeight = 0.f/box.GetWidth();
	for(int i = 0;i<graph->GetNumEdges();i++){
		int n = mControPoints[i].size();
		if(n<4) continue;

		float score1 = 0, score2 = 0;

		// this is for angle metrics
		MyVec3f dir1 = (mControPoints[i][1] - mControPoints[i][0]).normalized();
		MyVec3f dir2 = (mControPoints[i][2] - mControPoints[i][1]).normalized();
		MyVec3f dir3 = (mControPoints[i][3] - mControPoints[i][2]).normalized();
		float angle0 = acos(dir1*dir2);
		float angle1 = acos(dir2*dir3);

		for(int k = 4;k<n-1;k++){
			MyVec3f dir4 = (mControPoints[i][k] - mControPoints[i][k-1]).normalized();
			float angle2 = acos(dir3*dir4);

			if(SIGN(angle2-angle1) != SIGN(angle1-angle0)){
				score1 += -1.f;
			}

			angle0 = angle1;
			angle1 = angle2;

			dir3 = dir4;
		}

		// for distance metrics
		MyVec2i edge = graph->GetEdge(i);
		MyLine3f edgeLine(mLayout->GetNodePos(edge[0]), mLayout->GetNodePos(edge[1]));
		MyVec3f dir = edgeLine.GetUnitVector();
		for(int k = 1;k<n-1;k++){
			MyVec3f pDir = mControPoints[i][k] - edgeLine.GetStart();
			float distance = fabs((dir^pDir)[2]);
			score2 += -distance;
		}

		float score = angleWeight*score1 + distanceWeight*score2;

		//std::cout << i << " angle: "<< score1 << " dist: "<< score2 << " total: " << score << endl;

		if(score<-50){
			edgeToBeFixed << i;
		}
	}

	float distToSearch = 20;
	// fix the edges
	for(int i = 0;i<edgeToBeFixed.size();i++){
		//mControPoints[edgeToBeFixed[i]].clear();
		//MyVec2i edge = mInGraph->GetEdge(edgeToBeFixed[i]);
		//mControPoints[edgeToBeFixed[i]] << mPos->at(edge[0]);
		//mControPoints[edgeToBeFixed[i]] << mPos->at(edge[1]);

	}

	// basic spline smooth
	this->SmoothControlPoints(25+2, 0.5);
}

void MyGraphGeometryEdgeBundler::ShowDebug() const{
	return;
	MyArray<MyArrayi> cellToMerged(mDivisionX,mDivisionY);
	for(int i = 0;i<mergedGrids.size();i++){
		for(int j = 0;j<mergedGrids[i].size();j++){
			MyVec2i index = mergedGrids[i][j];
			cellToMerged[index[0]][index[1]] = i;
		}
	}
	
	for(int x = 0;x<mDivisionX;x++){
		for(int y = 0;y<mDivisionY;y++){
			int mergedIdx = cellToMerged[x][y];
			MyVec2f lowLeft = grid.GetLowPos(MyVec2i(x,y));
			/*
			MyGraphDrawer::Color(MyColor4f::white());
			// down
			if(y>0){
				int neighbourIdx = cellToMerged[x][y-1];
				if(mergedIdx != neighbourIdx){
					MyGraphDrawer::DrawLineAt(MyVec3f(lowLeft[0],lowLeft[1],0),
						MyVec3f(lowLeft[0]+grid.GetCellWidth(),lowLeft[1],0));
				}
			}
			else{
				MyGraphDrawer::DrawLineAt(MyVec3f(lowLeft[0],lowLeft[1],0),
					MyVec3f(lowLeft[0]+grid.GetCellWidth(),lowLeft[1],0));
			}
			//left
			if(x>0){
				int neighbourIdx = cellToMerged[x-1][y];
				if(mergedIdx != neighbourIdx){
					MyGraphDrawer::DrawLineAt(MyVec3f(lowLeft[0],lowLeft[1],0),
						MyVec3f(lowLeft[0],lowLeft[1]+grid.GetCellHeight(),0));
				}
			}
			else{
				MyGraphDrawer::DrawLineAt(MyVec3f(lowLeft[0],lowLeft[1],0),
					MyVec3f(lowLeft[0],lowLeft[1]+grid.GetCellHeight(),0));
			}
			// up
			if(y<mDivisionY-1){
				int neighbourIdx = cellToMerged[x][y+1];
				if(mergedIdx != neighbourIdx){
					MyGraphDrawer::DrawLineAt(MyVec3f(lowLeft[0],lowLeft[1]+grid.GetCellHeight(),0),
						MyVec3f(lowLeft[0]+grid.GetCellWidth(),lowLeft[1]+grid.GetCellHeight(),0));
				}
			}
			else{
				MyGraphDrawer::DrawLineAt(MyVec3f(lowLeft[0],lowLeft[1]+grid.GetCellHeight(),0),
					MyVec3f(lowLeft[0]+grid.GetCellWidth(),lowLeft[1]+grid.GetCellHeight(),0));
			}
			//right
			if(x<mDivisionX-1){
				int neighbourIdx = cellToMerged[x+1][y];
				if(mergedIdx != neighbourIdx){
					MyGraphDrawer::DrawLineAt(MyVec3f(lowLeft[0]+grid.GetCellWidth(),lowLeft[1],0),
						MyVec3f(lowLeft[0]+grid.GetCellWidth(),lowLeft[1]+grid.GetCellHeight(),0));
				}
			}
			else{
				MyGraphDrawer::DrawLineAt(MyVec3f(lowLeft[0]+grid.GetCellWidth(),lowLeft[1],0),
					MyVec3f(lowLeft[0]+grid.GetCellWidth(),lowLeft[1]+grid.GetCellHeight(),0));
			}
			float angle = mainAngles[x][y];
			angle = gridMainAngles[mergedIdx];
			if(angle > 0){
				MyVec2f offset(grid.GetCellWidth()/2,grid.GetCellWidth()/2*tanf(angle));
				offset.normalize();
				offset*=2;
				MyGraphDrawer::Color(MyColor4f::yellow());
				MyGraphDrawer::DrawLineAt(MyVec3f(lowLeft[0]+grid.GetCellWidth()/2,lowLeft[1]+grid.GetCellHeight()/2,0),
					MyVec3f(lowLeft[0]+grid.GetCellWidth()/2+offset[0],lowLeft[1]+grid.GetCellHeight()/2+offset[1],0));
				//MyGraphDrawer::DrawBitMapText(MyVec3f(lowLeft[0]+grid.GetCellWidth()/2,lowLeft[1]+grid.GetCellHeight()/2,0.f),
				//	MyString(angle));
			}
			*/
		}
	}
	
	
	for(int i = 0;i<mergedGrids.size();i++){
		
		MyGraphDrawer::Color(MyColor4f(i/100.f,(i%20)/20.f,(i%17)/17.f,0));
		MyArray2f* contour = grid.MakeContourArray(mergedGrids[i]);
		MyVec3f up(0,0,1);
		MyGraphDrawer::BeginLineLoop();
		int n = contour->size();
		for(int j = 0;j<n;j++){
			MyVec2f diff1 = contour->at((j+1)%n) - contour->at(j);
			MyVec3f dir1(diff1[0], diff1[1], 0);
			MyVec3f right1 = dir1^up;
			MyVec3f offset13 = right1.normalized()*grid.GetCellWidth()/10;
			MyVec2f offset12(offset13[0], offset13[1]);
			
			MyVec2f diff2 = contour->at(j) - contour->at((j-1+n)%n);
			MyVec3f dir2(diff2[0], diff2[1], 0);
			MyVec3f right2 = dir2^up;
			MyVec3f offset23 = right2.normalized()*grid.GetCellHeight()/10;
			MyVec2f offset22(offset23[0], offset23[1]);

			MyVec2f offset = (offset12+offset22).normalized()*grid.GetCellHeight()/10;
			MyGraphDrawer::Vertex(contour->at(j)+offset);
		}
		MyGraphDrawer::EndPrimitive();
		delete contour;
	}
	
	MyGraphDrawer::SetLineWidth(5);
	for(int i = 0;i<gridLine.size();i++){
		MyGraphDrawer::DrawLine(gridLine[i]);
	}
	MyGraphDrawer::SetLineWidth(1);

	MyGraphDrawer::Color(MyColor4f::yellow());
	for(int i = 0;i<meshLines.size();i++){
		MyGraphDrawer::DrawLine(meshLines[i]);
	}

	for(int i = 0;i<intersections.size();i++){
		MyGraphDrawer::DrawSphereAt(intersections[i].toDim<3>(0),0.5);
	}

	//MyGraphDrawer::Color(MyColor4f::yellow());
	for(int i = 0;i<edgeToBeFixed.size();i++){
		//MyPolyline3f polyline;
		//polyline.SetLine(mControPoints[edgeToBeFixed[i]]);
		//MyGraphDrawer::Draw(polyline);
	}
	
}