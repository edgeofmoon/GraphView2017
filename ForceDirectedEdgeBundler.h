#pragma once

#include <string>

class ForceDirectedEdgeBundler
{
public:
	ForceDirectedEdgeBundler(){};
	~ForceDirectedEdgeBundler(){};

	typedef struct EP{
		float x, y;
		std::string ptName;
	} Point;

	typedef struct V2D{
		float x;
		float y;
		float length;
	} Vector2D;

	typedef struct Pas{
		int numCycles;
		int P;    // initial number of subdivision points.
		float S;   // step size - shouldn't be higher than 1.0.
		int I;     // number of iteration steps performed during a cycle.
		float K; // global spring constant (used to control the amount of edge bundling by.
		// determining the stiffness of the edges).
		float stepDampingFactor;
		float edgeCompatibilityThreshold;
		bool directionAffectsCompatibility;
		bool binaryCompatibility;
		bool useInverseQuadraticModel;
		bool useRepulsionForOppositeEdges; // for compatible edges going into opposite directions.
		bool useSimpleCompatibilityMeasure;
		bool edgeValueAffectsAttraction;
		//  private boolean joinCloseSubdivisionPoints;
		float repulsionAmount;
		float subdivisionPointsCycleIncreaseRate;
		bool updateViewAfterEachStep;
	} Params;

	typedef struct CE{
		int edgeIdx;
		float C;
	} CompatibleEdge;

	
	Params resetToDefaults();
	Params params;
	float computeDistance(Point &p1,Point &p2);
	std::vector<Point> endPoints;
	std::vector<Point> edgeStarts;
	std::vector<Point> edgeEnds;
	std::vector<float> edgeValues;
	std::vector<float> edgeLengths;
	float edgeValueMax, edgeValueMin;
	int P;    // number of subdivision points (will increase with every cycle).
	float Pfloat; // used to keep the float value to keep the stable increase rate.
	float S;   // step size.
	int I;    // number of iteration steps performed during a cycle.
	
	std::vector<std::vector<CompatibleEdge>> compatibleEdgeLists;
	int numPoints;
	int numEdges;
	
	static const float EPS;
	std::vector<std::vector<Point>> edgePoints;
	int cycle;
	std::vector<std::vector<float>> compatibility_h;

	void bundle();
	void calcEdgeCompatibilityMeasures();
protected:

private:
	Vector2D &Vector(float x,float y,Vector2D &v);
	float dot(Vector2D &a,Vector2D &b);
	Vector2D &valueOf(Point &startPoint, Point &endPoint,Vector2D &v);
	float length(Vector2D &v);
	bool isSelfLoop(int edgeIdx);
	float signum(float x);
	float calcSimpleEdgeCompatibility(int i, int j);
	Point &between(Point &a, Point &b, float alpha, Point &np);
	Point between(Point &a, Point &b, float alpha);
	Point &midpoint(Point &a, Point &b, Point &np);
	Point& projectPointToLine(float x1, float y1, float x2, float y2, float x, float y, Point &np);
	Point& projectPointToLine(Point &line1, Point &line2, Point &point, Point &np);
	float visibilityCompatibility(Point &p0, Point &p1, Point &q0, Point &q1);
	float calcStandardEdgeCompatibility(int i, int j);
	float calcEdgeCompatibility(int i, int j);
	void addSubdivisionPoints(int P);
	void nextCycle();
	void initPoints();
	void initValues();
	void init();
	void writeGraph();
	int mainTest();

};