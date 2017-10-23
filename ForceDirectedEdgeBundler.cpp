/*
 * Force-Directed Edge Bundling using C++.
 *
 *
 *
 */
 
#include <iostream>
#include <vector>
#include <string>
#include <math.h>
#include <time.h>
#include <algorithm>
#include "ForceDirectedEdgeBundler.h"

using namespace std;

#define CLOCKS_PER_SECOND 1024

const float ForceDirectedEdgeBundler::EPS = 1e-7;

float ForceDirectedEdgeBundler::computeDistance(Point &p1,Point &p2)
{
    float dx=p1.x-p2.x;
    float dy=p1.y-p2.y;
    return sqrt(dx*dx+dy*dy);
}

//********************************************************************


ForceDirectedEdgeBundler::Vector2D& ForceDirectedEdgeBundler::Vector(float x,float y,Vector2D &v)
{
    v.x=x;
    v.y=y;
    return v;
}

float ForceDirectedEdgeBundler::dot(Vector2D &a,Vector2D &b)
{
    return a.x*b.x+a.y*b.y;
}

ForceDirectedEdgeBundler::Vector2D& ForceDirectedEdgeBundler::valueOf(Point &startPoint, Point &endPoint,Vector2D &v)
{
    return Vector(endPoint.x- startPoint.x, endPoint.y- startPoint.y,v);
}

float ForceDirectedEdgeBundler::length(Vector2D &v)
{
    float length;
    length = sqrt(v.x * v.x + v.y * v.y);
    return length;
}

ForceDirectedEdgeBundler::Params ForceDirectedEdgeBundler::resetToDefaults()
{
    Params p;
    p.updateViewAfterEachStep=true;
    p.numCycles=10;
    p.P=1;
    p.S=1.5;
    p.I=100;
    p.K=0.1;
    p.repulsionAmount=1.0;
    p.stepDampingFactor=0.5;
    p.edgeCompatibilityThreshold=0.7;
    p.directionAffectsCompatibility=true;
    p.binaryCompatibility=false;
    p.useInverseQuadraticModel=false;
    p.useRepulsionForOppositeEdges=true;
    p.useSimpleCompatibilityMeasure=false;
    p.edgeValueAffectsAttraction=true;
    p.subdivisionPointsCycleIncreaseRate=1.3;
    return p;
}

//********************************************************************
bool ForceDirectedEdgeBundler::isSelfLoop(int edgeIdx)
{
    return edgeLengths[edgeIdx] == 0.0;
}

float ForceDirectedEdgeBundler::signum(float x)
{
    if (x == 0.0) return 0;
    else if (x < 0.0) return -1.0;
    else return 1.0;
}

//********************************************************************
float ForceDirectedEdgeBundler::calcSimpleEdgeCompatibility(int i, int j)
{
    if (isSelfLoop(i)  ||  isSelfLoop(j))
    {
        return 0.0;
    }

    float l_avg = (edgeLengths[i] + edgeLengths[j])/2;
    return l_avg / (l_avg +
                    computeDistance(edgeStarts[i],edgeStarts[j]) +
                    computeDistance(edgeEnds[i],edgeEnds[j]));
}


//returns a point on a segment between the two points.
ForceDirectedEdgeBundler::Point& ForceDirectedEdgeBundler::between(Point &a, Point &b, float alpha, Point &np)
{
    np.x=a.x + (b.x - a.x) * alpha;
    np.y=a.y + (b.y - a.y) * alpha;
    return np;
}

ForceDirectedEdgeBundler::Point ForceDirectedEdgeBundler::between(Point &a, Point &b, float alpha)
{
	Point np;
    np.x=a.x + (b.x - a.x) * alpha;
    np.y=a.y + (b.y - a.y) * alpha;
    return np;
}

ForceDirectedEdgeBundler::Point& ForceDirectedEdgeBundler::midpoint(Point &a, Point &b, Point &np)
{
    return between(a, b, 0.5, np);
}


ForceDirectedEdgeBundler::Point& ForceDirectedEdgeBundler::projectPointToLine(float x1, float y1, float x2, float y2, float x, float y, Point &np)
{
    float L = sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
    float r = ((y1-y)*(y1-y2) - (x1-x)*(x2-x1)) / (L * L);
        
    np.x=x1 + r * (x2-x1);
    np.y=y1 + r * (y2-y1);
    return np;
}

ForceDirectedEdgeBundler::Point& ForceDirectedEdgeBundler::projectPointToLine(Point &line1, Point &line2, Point &point, Point &np)
{
    return projectPointToLine(
               line1.x, line1.y, line2.x, line2.y,
               point.x, point.y, np);
}

//********************************************************************
float ForceDirectedEdgeBundler::visibilityCompatibility(Point &p0, Point &p1, Point &q0, Point &q1)
{
    Point i0, i1, im, pm;
        
    i0 = projectPointToLine(p0, p1, q0, i0);
    i1 = projectPointToLine(p0, p1, q1, i1);
    im = midpoint(i0, i1, im);
    pm = midpoint(p0, p1, pm);
        
    float c=1 - 2 * computeDistance(pm,im) / computeDistance(i0,i1);
    return max(0.0, (double)c);
}

//********************************************************************
float ForceDirectedEdgeBundler::calcStandardEdgeCompatibility(int i, int j)
{
    if (isSelfLoop(i)  ||  isSelfLoop(j))
    {
        return 0.0;
    }
    Vector2D p,q;
    Point pm,qm;
        
    p = valueOf(edgeStarts[i], edgeEnds[i], p);
    q = valueOf(edgeStarts[j], edgeEnds[j], q);
    pm = midpoint(edgeStarts[i], edgeEnds[i], pm);
    qm = midpoint(edgeStarts[j], edgeEnds[j], qm);
    float l_avg = (edgeLengths[i] + edgeLengths[j])/2;

    // angle compatibility.
    float Ca;
    if (params.directionAffectsCompatibility)
    {
        Ca = (dot(p,q) / (length(p) * length(q)) + 1.0) / 2.0;
    }
    else
    {
        Ca = abs(dot(p,q) / (length(p) * length(q)));
    }
    if (abs(Ca) < EPS)
    {
        Ca = 0.0;    // this led to errors (when Ca == -1e-12).
    }
    if (abs(abs(Ca) - 1.0) < EPS)
    {
        Ca = 1.0;
    }

    // scale compatibility.
    float Cs = 2 / (
                    (l_avg / min(edgeLengths[i], edgeLengths[j]))  +
                    (max(edgeLengths[i], edgeLengths[j]) / l_avg)
                );

    // position compatibility.
    float Cp = l_avg / (l_avg + computeDistance(pm,qm));

    // visibility compatibility.
    float Cv;
    if (Ca * Cs * Cp > .9)
    {
        Cv = min(
                 visibilityCompatibility(edgeStarts[i], edgeEnds[i], edgeStarts[j], edgeEnds[j]),
                 visibilityCompatibility(edgeStarts[j], edgeEnds[j], edgeStarts[i], edgeEnds[i])
             );
    }
    else
    {
        Cv = 1.0;
    }

    if(!(Ca >= 0  &&  Ca <= 1))
    {
        printf("Ca wrong");
        exit;
    }
    if(!(Cs >= 0  &&  Cs <= 1))
    {
        printf("Cs wrong");
        exit;
    }
    if(!(Cp >= 0  &&  Cp <= 1))
    {
        printf("Cp wrong");
        exit;
    }
    if(!(Cv >= 0  &&  Cv <= 1))
    {
        printf("Cv wrong");
        exit;
    }

    if (params.binaryCompatibility)
    {
        float threshold = params.edgeCompatibilityThreshold;
        Ca = Ca >= threshold ? 1.0 : 0.0;
        Cs = Cs >= threshold ? 1.0 : 0.0;
        Cp = Cp >= threshold ? 1.0 : 0.0;
        Cv = Cv >= threshold ? 1.0 : 0.0;
    }

    return Ca * Cs * Cp * Cv;
}

//********************************************************************
float ForceDirectedEdgeBundler::calcEdgeCompatibility(int i, int j)
{
    float C;
    if (params.useSimpleCompatibilityMeasure)
    {
        C = calcSimpleEdgeCompatibility(i, j);
    }
    else
    {
        C = calcStandardEdgeCompatibility(i, j);
    }
    if(!(C >= 0  &&  C <= 1.0))
    {
        printf("C wrong\n");
        exit;
    }
    if (params.binaryCompatibility)
    {
        if (C >= params.edgeCompatibilityThreshold)
        {
            C = 1.0;
        }
        else
        {
            C = 0.0;
        }
    }
        if (params.useRepulsionForOppositeEdges)
        {
                Vector2D p,q;
                p = valueOf(edgeStarts[i], edgeEnds[i], p);
                q = valueOf(edgeStarts[j], edgeEnds[j], q);
                float cos = dot(p,q) / (length(p) * length(q));
                if (cos < 0)
                {
                        C = -C;
                }
        }
    return C;
}

//********************************************************************
void ForceDirectedEdgeBundler::calcEdgeCompatibilityMeasures()
{
    int numTotal = 0;
    int numCompatible = 0;
    float Csum = 0;
        int i;
    CompatibleEdge ce;
	compatibility_h.resize(numEdges);
        for (i = 0; i < numEdges; i++){
                compatibleEdgeLists[i].begin();
        }
    for (i = 0; i < numEdges; i++)
    {
		if(i%100==0)
			cout << i << endl;
		compatibility_h[i].resize(numEdges);
        for (int j = 0; j < i; j++)
        {
            float C = calcEdgeCompatibility(i, j);
            if (abs(C) >=params.edgeCompatibilityThreshold)
            {
                ce.edgeIdx=j;
                ce.C=C;
                                compatibleEdgeLists[i].push_back(ce);
                ce.edgeIdx=i;
                                compatibleEdgeLists[j].push_back(ce);
                numCompatible++;

                                compatibility_h[i][j]=C;
            }
            Csum += abs(C);
            numTotal++;
        }

    }

}

//********************************************************************
void ForceDirectedEdgeBundler::addSubdivisionPoints(int P)
{
    int prevP = 0;
    if (edgePoints.size() == 0)
    {
        prevP = 0;
    }
    else
    {
		for(int i = 0;i<edgePoints.size();i++){
			if(edgePoints[i].size()>0){
				prevP = edgePoints[i].size();
				break;
			}
		}
    }
    // bigger array for subdivision points of the next cycle.
    vector<vector<Point>> newEdgePoints(numEdges);

    // add subdivision points.
    for (int i = 0; i < numEdges; i++)
    {
        if (isSelfLoop(i))
        {
            continue;   // ignore self-loops.
        }
        vector<Point> newPoints(newEdgePoints[i]);
                newPoints.begin();
        if (cycle == 0)
        {
            if(P != 1)
            {
                printf("wrong\n");
                exit;
            };
                        Point mp;
                        mp=midpoint(edgeStarts[i], edgeEnds[i], mp);
                        newPoints.push_back(mp);
                        newEdgePoints[i]=newPoints;
        }
        else
        {
            vector<Point> points = edgePoints[i];
            points.insert(points.begin(),edgeStarts[i]);
                        points.back();
                        points.push_back(edgeEnds[i]);

            float polylineLen = 0;
            vector<float> segmentLen;
            for (int j = 0; j < prevP + 1; j++)
            {
                float segLen = computeDistance(points[j],points[j+1]);
                                segmentLen.push_back(segLen);
                polylineLen += segLen;
            }

            float L = polylineLen / (P + 1);
            int curSegment = 0;
            float prevSegmentsLen = 0;
            Point p = points[0];
            Point nextP = points[1];
            for (int j = 0; j < P; j++)
            {
                while (segmentLen[curSegment] < L * (j + 1) - prevSegmentsLen)
                {
                    prevSegmentsLen += segmentLen[curSegment];
                    curSegment++;
                    p = points[curSegment];
                    nextP = points[curSegment + 1];
                }
                float d = L * (j + 1) - prevSegmentsLen;
                                newPoints.push_back(between(p, nextP, d / segmentLen[curSegment]));
            }
                        newEdgePoints[i]=newPoints;

        }
    }
    edgePoints = newEdgePoints;
}

//********************************************************************
void ForceDirectedEdgeBundler::nextCycle()
{
    // set parameters for the next cycle.
    if (cycle > 0)
    {
        Pfloat *= params.subdivisionPointsCycleIncreaseRate;
        P = (int)(Pfloat+0.5);
        S *= (1.0 - params.stepDampingFactor);
        I = (I * 2) / 3;
    }
    addSubdivisionPoints(P);

    vector <vector<Point>> tmpEdgePoints(numEdges);

        for(int step=0; step<I; step++)
    {
        for(int pe=0; pe<numEdges; pe++)
        {
                        
            vector<Point> p(edgePoints[pe]);
            vector<Point> newP;
            if (isSelfLoop(pe))
            {
                continue;     // ignore self-loops.
            }
            int numOfSegments = P + 1;
            float k_p = params.K / (edgeLengths[pe] * numOfSegments);
            vector<CompatibleEdge> compatible = compatibleEdgeLists[pe];
            for (int i = 0; i < P; i++)
            {
                // spring forces.
                Point p_i = p[i];
                Point p_prev = (i == 0 ? edgeStarts[pe] : p[i - 1]);
                Point p_next = (i == P - 1 ? edgeEnds[pe] : p[i + 1]);
                float Fsi_x = (p_prev.x - p_i.x) + (p_next.x - p_i.x);
                float Fsi_y = (p_prev.y - p_i.y) + (p_next.y - p_i.y);

                if (abs(k_p) < 1.0)
                {
                    Fsi_x *= k_p;
                    Fsi_y *= k_p;
                }

                // attracting electro forces (for each other compatible edge).
                float Fei_x = 0;
                float Fei_y = 0;
                                int size=compatible.size();
                for (int ci = 0; ci < size; ci++)
                {
                    CompatibleEdge ce = compatible[ci];
                    int qe = ce.edgeIdx;
                    float C = ce.C;
                    Point q_i = edgePoints[qe][i];

                    float v_x = q_i.x - p_i.x;
                    float v_y = q_i.y - p_i.y;
                    if (abs(v_x) > EPS  ||  abs(v_y) > EPS)    // zero vector has no direction.
                    {
                        float d = sqrt(v_x * v_x + v_y * v_y);  // shouldn't be zero.
                        float m;
                        if (params.useInverseQuadraticModel)
                        {
                            m = (C / d) / (d * d);
                        }
                        else
                        {
                            m = (C / d) / d;
                        }
                        if (C < 0)    // means that repulsion is enabled.
                        {
                            m *= params.repulsionAmount;
                        }
                        if (params.edgeValueAffectsAttraction)
                        {
                            float coeff = 1.0 + max(-1.0, (double)(edgeValues[qe] - edgeValues[pe])/(edgeValueMax + edgeValueMin));
                            m *= coeff;
                        }
                        if (abs(m * S) > 1.0)    // this condition is to reduce the "hairy" effect.
                        {
                            m = signum(m) / S;
                        }
                        v_x *= m;
                        v_y *= m;
                        Fei_x += v_x;
                        Fei_y += v_y;
                    }
                }

                float Fpi_x = Fsi_x + Fei_x;
                float Fpi_y = Fsi_y + Fei_y;

                Point np;
                np.x=p[i].x;
                np.y=p[i].y;
                np.x=np.x + Fpi_x * S;
                np.y=np.y + Fpi_y * S;
                                newP.push_back(np);
                                
            }
                        tmpEdgePoints[pe]=newP;
        }
        //update position.
        edgePoints=tmpEdgePoints;
    }
    cycle++;
}

//********************************************************************
void ForceDirectedEdgeBundler::bundle()
{
    // iterative refinement scheme.
    int numCycles = params.numCycles;
    for (int cycle = 0; cycle < numCycles; cycle++)
    {
		cout << "cycle= "<< cycle<< endl;
        nextCycle();
    }
}

//********************************************************************
void ForceDirectedEdgeBundler::initPoints()
{
	/*
    FILE *f;
    f=fopen("D://cord_cord.txt","r");//FIXME
    float x=0,y=0;
    Point p;
    for(int i=0; i<numPoints; i++)
    {
        fscanf(f,"%f,",&x);
        fscanf(f,"%f",&y);
        p.x=x;
        p.y=y;
        endPoints.push_back(p);
    }
    fclose(f);
	*/
    for(int i=0; i<numPoints; i++)
    {
		Point p;
		p.x = rand()%1000/1000.f;
		p.y = rand()%1000/1000.f;
        endPoints.push_back(p);
	}

}

//********************************************************************
void ForceDirectedEdgeBundler::initValues()
{
    float evMin = pow(10.0,10.0), evMax = -pow(10.0,10.0);
    //FILE *f;
    //f=fopen("D://mig2005.txt","r");//FIXME
    int i,j;
    int _value;
    float length;
    //char v[256];

    //fscanf(f,"%s",v);
    //fgetc(f);
    for(i=1; i<numPoints; i++)
    {
        for(j=0; j<i-1; j++)
        {
            edgeStarts.push_back(endPoints[i]);
            edgeEnds.push_back(endPoints[j]);
            length = computeDistance(endPoints[i],endPoints[j]);
            if (abs(length) < EPS) length = 0.0;
            edgeLengths.push_back(length);
			//fscanf(f,"%d",&_value);
            //fgetc(f);
			float midval = rand()%1000/1000.f;
			_value = evMin+(evMax-evMin)*midval;
            edgeValues.push_back(_value);
            if(_value>evMax)
            {
                evMax=_value;
            }
            if(_value<evMin)
            {
                evMin=_value;
            }
        }
        edgeStarts.push_back(endPoints[i]);
        edgeEnds.push_back(endPoints[j]);
        length = computeDistance(endPoints[i],endPoints[j]);
        if (abs(length) < EPS) length = 0.0;
        edgeLengths.push_back(length);
        //fscanf(f,"%d",&_value);
        //fgetc(f);
        //fscanf(f,"%s",v);
        //fgetc(f);
		float midval = rand()%1000/1000.f;
		_value = evMin+(evMax-evMin)*midval;
        edgeValues.push_back(_value);
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
    if (params.edgeValueAffectsAttraction)
    {
        edgeValueMax = evMax;
        edgeValueMin = evMin;
    }
}

//********************************************************************
void ForceDirectedEdgeBundler::init()
{
    params=resetToDefaults();
    numPoints=31;
    numEdges=numPoints*(numPoints-1)/2;
    initPoints();
    initValues();
    I = params.I;
    P = params.P;
    Pfloat = P;
    S = params.S;
        compatibleEdgeLists=vector< vector<CompatibleEdge>> (numEdges);

    cycle = 0;
}

//********************************************************************
void ForceDirectedEdgeBundler::writeGraph()
{
        FILE *f;
        f=fopen("E://graph_bundler.txt","w");//FIXME
        int i;
        Point p_prev,p_next;
        vector<Point> p;
        for(int pe=0;pe<numEdges;pe++){
                p = edgePoints[pe];
                for(i=0;i<=P;i++){
                        p_prev = (i == 0 ? edgeStarts[pe]: p[i - 1]);
                        //p_next = (i == P ? edgeEnds[pe] : p[i]);
                        fprintf(f,"%f,%f,",p_prev.x,p_prev.y);
                }
                p_next = edgeEnds[pe];
                fprintf(f,"%f,%f",p_next.x,p_next.y);
                fprintf(f,"\n");
        }
        fclose(f);
}

//********************************************************************
int ForceDirectedEdgeBundler::mainTest()
{
        clock_t start,stop;
        float elapsedTime;
        
        //input.
    init();
        
        start=clock();//record start time.
        
        //bundle.
        calcEdgeCompatibilityMeasures();
    bundle();

        stop =clock();//record stop time.
        elapsedTime = (stop - start)*1000/CLOCKS_PER_SECOND;
        printf("time: %f ms\n",elapsedTime);//output used time.

        //output result.
        writeGraph();
        
        return 0;
} 
