#include "MyGraphEdgeBundler.h"
#include "ForceDirectedEdgeBundler.h"
#include "MyVec.h"
#include "MyGraphLayout.h"
#include "BSplineGA.h"
#include "MyPolyLine.h"
//#include <cassert>

MyGraphEdgeBundler::MyGraphEdgeBundler(void)
{
}


MyGraphEdgeBundler::~MyGraphEdgeBundler(void)
{
}


void MyGraphEdgeBundler::SetGraphLayout(const MyGraphLayout* layout){
	mLayout = layout;
}

const MyArray<MyArray3f>* MyGraphEdgeBundler::GetControPoints() const{
	return &mControPoints;
}

/*
void MyGraphEdgeBundler::Update(){
	ForceDirectedEdgeBundler bundler;
	bundler.params=bundler.resetToDefaults();
    bundler.numPoints=mInGraph->GetNumNodes();
    bundler.numEdges=bundler.numPoints*(bundler.numPoints-1)/2;
    //initPoints
	for(int i=0; i< bundler.numPoints; i++)
    {
		ForceDirectedEdgeBundler::Point p;
		p.x = mPos->at(i)[0];
		p.y = mPos->at(i)[1];
        bundler.endPoints.push_back(p);
	}

    //initValues;
	float evMin = pow(10.0,10.0), evMax = -pow(10.0,10.0);
    int i,j;
    int _value;
    float length;
    for(i=1; i<bundler.numPoints; i++)
    {
        for(j=0; j<i-1; j++)
        {
            bundler.edgeStarts.push_back(bundler.endPoints[i]);
            bundler.edgeEnds.push_back(bundler.endPoints[j]);
            length = bundler.computeDistance( bundler.endPoints[i], bundler.endPoints[j]);
            if (abs(length) < bundler.EPS) length = 0.0;
			bundler.edgeLengths.push_back(length);
			int edgeIdx = mInGraph->GetEdgeIndex(MyVec2i(i,j));
			float midval = fabs(mInGraph->GetEdgeWeight(edgeIdx));
			_value = evMin+(evMax-evMin)*midval;
            bundler.edgeValues.push_back(_value);
            if(_value>evMax)
            {
                evMax=_value;
            }
            if(_value<evMin)
            {
                evMin=_value;
            }
        }
        bundler.edgeStarts.push_back(bundler.endPoints[i]);
        bundler.edgeEnds.push_back(bundler.endPoints[j]);
        length = bundler.computeDistance(bundler.endPoints[i],bundler.endPoints[j]);
        if (abs(length) < bundler.EPS) length = 0.0;
        bundler.edgeLengths.push_back(length);
        //fscanf(f,"%d",&_value);
        //fgetc(f);
        //fscanf(f,"%s",v);
        //fgetc(f);
		int edgeIdx = mInGraph->GetEdgeIndex(MyVec2i(i,j));
		float midval = fabs(mInGraph->GetEdgeWeight(edgeIdx));
		_value = evMin+(evMax-evMin)*midval;
        bundler.edgeValues.push_back(_value);
        if(_value>evMax)
        {
            evMax=_value;
        }
        if(_value<evMin)
        {
            evMin=_value;
        }
    }
    //fclose(f);
    if (bundler.params.edgeValueAffectsAttraction)
    {
        bundler.edgeValueMax = evMax;
        bundler.edgeValueMin = evMin;
    }


    bundler.I = bundler.params.I;
    bundler.P = bundler.params.P;
    bundler.Pfloat = bundler.P;
    bundler.S = bundler.params.S;
    bundler.compatibleEdgeLists=std::vector<std::vector<ForceDirectedEdgeBundler::CompatibleEdge>>(bundler.numEdges);

    bundler.cycle = 0;

	
    bundler.calcEdgeCompatibilityMeasures();
    bundler.bundle();

	// write edges
	mControPoints.clear();
	ForceDirectedEdgeBundler::Point p_prev,p_next;
	std::vector<ForceDirectedEdgeBundler::Point> p;
	for(int pe=0;pe<bundler.numEdges;pe++){
		p = bundler.edgePoints[pe];
		MyArray3f arr;
		for(int i=0;i<=bundler.P;i++){
			p_prev = (i == 0 ? bundler.edgeStarts[pe]: p[i - 1]);
			arr.push_back(MyVec3f(p_prev.x,p_prev.y,0));
		}
		p_next = bundler.edgeEnds[pe];
		arr.push_back(MyVec3f(p_next.x,p_next.y,0));
		mControPoints.push_back(arr);
	}
}
*/

void MyGraphEdgeBundler::Update(){
	const MyGenericGraphf* graph = mLayout->GetGraph();
	ForceDirectedEdgeBundler bundler;
	bundler.params=bundler.resetToDefaults();
    bundler.numPoints=graph->GetNumNodes();
    bundler.numEdges=graph->GetNumEdges();
    //initPoints
	for(int i=0; i< bundler.numPoints; i++)
    {
		ForceDirectedEdgeBundler::Point p;
		p.x = mLayout->GetNodePos(i)[0];
		p.y = mLayout->GetNodePos(i)[1];
        bundler.endPoints.push_back(p);
	}

    //initValues;
	float evMin = pow(10.0,10.0), evMax = -pow(10.0,10.0);
    int i,j;
    int _value;
    float length;
    for(i=0; i<bundler.numPoints; i++)
    {
        for(j=0; j<bundler.numPoints; j++)
        {
			int edgeIdx = graph->GetEdgeIndex(MyVec2i(i,j));
			if(edgeIdx<0) continue;
            bundler.edgeStarts.push_back(bundler.endPoints[i]);
            bundler.edgeEnds.push_back(bundler.endPoints[j]);
            length = bundler.computeDistance( bundler.endPoints[i], bundler.endPoints[j]);
            if (abs(length) < bundler.EPS) length = 0.0;
			bundler.edgeLengths.push_back(length);
			float midval = fabs(graph->GetEdgeWeight(edgeIdx));
			_value = evMin+(evMax-evMin)*midval;
            bundler.edgeValues.push_back(_value);
            if(_value>evMax)
            {
                evMax=_value;
            }
            if(_value<evMin)
            {
                evMin=_value;
            }
        }
    }
    //fclose(f);
    if (bundler.params.edgeValueAffectsAttraction)
    {
        bundler.edgeValueMax = evMax;
        bundler.edgeValueMin = evMin;
    }


    bundler.I = bundler.params.I;
    bundler.P = bundler.params.P;
    bundler.Pfloat = bundler.P;
    bundler.S = bundler.params.S;
    bundler.compatibleEdgeLists=std::vector<std::vector<ForceDirectedEdgeBundler::CompatibleEdge>>(bundler.numEdges);

    bundler.cycle = 0;

	
    bundler.calcEdgeCompatibilityMeasures();
    bundler.bundle();

	// write edges
	mControPoints.clear();
	ForceDirectedEdgeBundler::Point p_prev,p_next;
	std::vector<ForceDirectedEdgeBundler::Point> p;
	for(int pe=0;pe<bundler.numEdges;pe++){
		p = bundler.edgePoints[pe];
		MyArray3f arr;
		for(int i=0;i<=bundler.P && i<=p.size();i++){
			p_prev = (i == 0 ? bundler.edgeStarts[pe]: p[i - 1]);
			arr.push_back(MyVec3f(p_prev.x,p_prev.y,0));
		}
		p_next = bundler.edgeEnds[pe];
		arr.push_back(MyVec3f(p_next.x,p_next.y,0));
		mControPoints.push_back(arr);
	}
}


void MyGraphEdgeBundler::SmoothControlPoints(int numPoints, float strength){
	return;
	// smooth edges
	for(int iEdge = 0;iEdge<mControPoints.size();iEdge++){
		if(mControPoints[iEdge].size()<=2){
			continue;
		}

		while(mControPoints[iEdge].size()<numPoints){
			MyArray3f newControlPoints(mControPoints[iEdge].size()+1);
			newControlPoints[0] = mControPoints[iEdge][0];
			for(int i = 1;i<newControlPoints.size()-1;i++){
				newControlPoints[i] = (1-strength)*mControPoints[iEdge][i-1]
				+strength*mControPoints[iEdge][i];
			}
			newControlPoints.back() = mControPoints[iEdge].back();
			mControPoints[iEdge] = newControlPoints;
		}

		//debug
		//MyVec2i edge = mInGraph->GetEdge(iEdge);
		//assert(mControPoints[iEdge][0] == mPos->at(edge[0]));
		//assert(mControPoints[iEdge].back() == mPos->at(edge[1]));
	}
	
	/*
	// smooth edges
	for(int iEdge = 0;iEdge<mControPoints.size();iEdge++){
		if(mControPoints[iEdge].size()<=4){
			continue;
		}

		std::vector< Vec3f> cps;
		for(int ip = 0;ip<mControPoints[iEdge].size();ip++){
			MyVec3f p = mControPoints[iEdge][ip];
			cps.push_back(Vec3f(p[0],p[1],p[2]));
		}
		CBSplineGA spline;
		bool suc = spline.Create(cps, 0.5,3);
		assert(suc);
		suc = spline.LeastSquaresApp();
		assert(suc);

		mControPoints[iEdge].resize(numPoints);
		for(int ip = 0;ip<numPoints;ip++){
			Vec3f p = spline.CurvePoint((float)ip/(numPoints-1));
			mControPoints[iEdge][ip] = MyVec3f(p[0],p[1],p[2]);
		}
		//debug
		//MyVec2i edge = mInGraph->GetEdge(iEdge);
		//assert(mControPoints[iEdge][0] == mPos->at(edge[0]));
		//assert(mControPoints[iEdge].back() == mPos->at(edge[1]));
	}
	*/
	/*
	// smooth edges
	for(int iEdge = 0;iEdge<mControPoints.size();iEdge++){
		if(mControPoints[iEdge].size()<=3){
			continue;
		}

		MyPolyline3f polyline(mControPoints[iEdge]);

		MyPolyline3f* curve = polyline.MakeBezierCurve(numPoints);

		mControPoints[iEdge] = curve->GetVertices();

		delete curve;
		//debug
		//MyVec2i edge = mInGraph->GetEdge(iEdge);
		//assert(mControPoints[iEdge][0] == mPos->at(edge[0]));
		//assert(mControPoints[iEdge].back() == mPos->at(edge[1]));
	}
	*/
}