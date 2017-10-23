#ifndef __LABEL_H
#define __LABEL_H

#include <math.h>
#include <vector>
#include <string>

#include "MyVec.h"
#include "MyColor4.h"

typedef MyVec3f Point;
typedef MyColor4f Color;

//---------------------------------------------------------------------------------------------------------
//  [1/6/2013 XLM]
//
// The labeling method comes from paper: "Particel-Based Labeling: Fast Point-Feature Labeling 
// without Obscuring Other Visual Features"
// I implemented "Vector Based" approach here.
//
//---------------------------------------------------------------------------------------------------------

struct BBox
{
	Point lt, rb;
	BBox(Point lefttop, Point rightbottom) : lt(lefttop), rb(rightbottom){}
	~BBox(){}

	bool Intersect(const BBox &box, float &dx, float &dy) const
	{
// 
// 		float x1 = lt.x;
// 		float x2 = rb.x;
// 		float y1 = lt.y;
// 		float y2 = rb.y;
// 		
// 		float x3 = box.lt.x;
// 		float x4 = box.rb.x;
// 		float y3 = box.lt.y;
// 		float y4 = box.rb.y;
// 
// // 		return ( ( (x1 >=x3 && x1 < x4) || (x3 >= x1 && x3 <= x2) ) &&
// // 			     ( (y1 >=y3 && y1 < y4) || (y3 >= y1 && y3 <= y2) ) ) ? true : fals
// 	
// 		if(fabs( (x1+x2)/2. - (x3+x4)/2. ) < ( (x2+x4-x1-x3)/2. )
// 			&& fabs( (y1+y2)/2. - (y3+y4)/2.) < ( (y2+y4-y1-y3)/2. )  )
// 			return true;
// 		else return false;
	
		float cx1,cy1,cx2,cy2;
		float w1,h1,w2,h2;
		w1 = rb[0]-lt[1];
		h1 = lt[1]-rb[1];
		w2 = box.rb[0]-box.lt[0];
		h2 = box.lt[1]-box.rb[1];

		w1/=2.;
		h1/=2.;
		w2/=2.;
		h2/=2.;

		cx1 = lt[0] + (w1);
		cy1 = lt[1] + (h1);

		cx2 = box.lt[0] + (w2);
		cy2 = box.lt[1] + (h2);

		return ( (fabs(cx1-cx2) <= (w1 + w2))
			&& ( fabs( cy1-cy2 ) <= (h1 + h2) ));
	}

	float GetWidth()  { return rb[0] - lt[0]; }
	float GetHeight() { return lt[1] - rb[1]; }
};


struct point_feature_t // the feature needs to be labels
{
	std::string name;
	Point       pos;

	point_feature_t(std::string n, Point p) : name(n), pos(p){}
};


struct label_t
{
	std::string str;  // the name of the label
	Point       pos;       // position of the first character of the label
	BBox        box;        // the rectangular of the label, initialize in its own coordinate
	bool        show;       // if this label has found a labeling place, show is true
	float       importance; // we draw label from higher importance to lower importance
	Color       color;

	label_t(const std::string s, const Point p, const BBox b, float im = 0, Color co = Color(255,255,255)) 
		   : str(s), pos(p), box(b), show(true), importance(im), color(co){}

	void setPos(Point p) 
	{
		pos = p;
		box.rb = box.rb - box.lt;  // modify the boundingbox coordinates simutaneously
		box.lt = Point(0,0);
		box.lt = box.lt + p;
		box.rb = box.rb + p;
	}
};


struct conflict_particle_t // sampled on visual features and labels, used to detect conflicts
{
	Point        pos;
	int     col, row;  // which grid cell it belongs to
	float        importance;   // for future usage

	conflict_particle_t(const Point p, const int c = 0, const int r = 0, const float impo = 0)
						: pos(p), col(c), row(r), importance(impo){}			 
};

#define CONFLICT_PARS      std::vector<conflict_particle_t>
#define CONFLICT_PARS_ITER std::vector<conflict_particle_t>::iterator

struct grid_elem_t  // grid element, a cell holds all conflict points in its area
{
	CONFLICT_PARS particles;
	int      row, col;          // the index of this cell in the grid

	grid_elem_t(int r, int c) : row(r), col(c){}
	void add(conflict_particle_t cp) { particles.push_back(cp); }
};


#define POINT_FEATURES     std::vector<point_feature_t>
#define LABELS             std::vector<label_t>
#define LABELS_ITER        std::vector<label_t>::iterator

#define CELLS              std::vector<grid_elem_t>
#define CELLS_ITER         std::vector<grid_elem_t>::iterator

struct spiral_param_t
{
	float mmax;  // total number of sampling points
	float c;     // number of rotations of the spiral
	float r;     // maximum radius
	int   d;       // the orientation of the spiral

	spiral_param_t(float mmax_ = 150.f, float c_ = 10.f, float r_ = 100.f, float d_ = -1)
				 : mmax(mmax_), c(c_), r(r_), d(d_){} 
	void setParams(float mmax_ = 150.f, float c_ = 10.f, float r_ = 100.f, float d_ = -1)
	{
		mmax = mmax_; c = c_; r = r_; d = d_;
	}
};

//
// Usage: 
// 1. LabelManager(w,h)
// 2. initialize(...)
// 3. [ AddConflictParticles(...) ]
// 4. Render()
//
//  function: Labeling() is the main pipeline to calculate labeling position
//

class LabelManager
{
public:
	LabelManager(){};
	LabelManager(int screen_w, int screen_h) : m_screen_height(screen_h), m_screen_width(screen_w){}
	~LabelManager(){}

	void AddConflictParticles(const std::vector<Point> &points);
	void AddConflictParticle(Point p);
	
	void Initialize(const std::vector< std::string > &names, std::vector<Point> positions);
	void Labeling();
	void Render();
	void UpdateFeaturePositions();

private:
	int m_screen_width;
	int m_screen_height;

	LABELS m_labels;
	std::vector< CELLS > m_grid;
	std::vector< std::vector<bool> > m_grid_covered; // indicate which grids are covered by a label
	POINT_FEATURES m_point_features;         // the positions that need to be labeled


	int m_cell_width;
	int m_cell_height;

	float m_max_label_width;
	float m_min_label_width;

	float m_max_label_height;
	float m_min_label_height;

	spiral_param_t m_param;

	bool m_computed;  // if calculated the labeling positions

	void CreateScreenGrid();
	void InitialzeLabels();
	int GetBitmapStringLength(std::string str);
	void CalcMaxMinLabelLength();
	void GetCellIndex(Point p, int &row, int &col);  // judge which cell the point is in
	void LabelCoverCell(const label_t &label);  // calculate those cells the label covers
	bool ConflictTest(const label_t &label);
	bool CheckFourPositionModel(const Point &pf, label_t &label);
	bool CheckEightPositionModel(const Point &pf, label_t &label);
	bool CheckDistantPosition(const Point &pf, label_t &label);
	Point SpiralFunc(float m); // space sampling function, used to sample distant labeling position
	void GenerateNewConflictParticlesForLabel(label_t &label);
	void RenderLines(Point &feature_pos, label_t &label); // draw a line connects point-feature and its label
	void DrawLine(Point &p1, Point &p2, Color c = Color(255,255,255,128)); // draw a line
	bool OutsideScreen(Point &p);
	//
	//  for debug
	//
	void RenderConflictParticles();
	void RenderSpaceFunc(Point &pf);
};
#endif //__LABEL_H
