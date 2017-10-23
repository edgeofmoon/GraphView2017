

#include "Label.h"

//#include "BundlesPrimitive.h"

#include <iostream>

#ifdef __APPLE__
  #include <GLUT/glut.h>
#else
  #include "gl/glut.h"
#endif

using namespace std;

extern int windowHeight;
extern Point compoudgraph_centre;

#define M_PI 3.14159265f

//  [1/6/2013 XLM]

void LabelManager::AddConflictParticles(const std::vector<Point> &points)
{
	if(m_grid.empty())
		return;

	for(int i = 0; i < points.size(); ++i)
	{
		AddConflictParticle(points[i]);
	}
}

void LabelManager::AddConflictParticle(Point p)
{
	int row, col;
	GetCellIndex(p, row, col);
	m_grid[row][col].add(p);
}

//
// names and positions should be sorted by their importance already
//
void LabelManager::Initialize(const std::vector< std::string > &names,std::vector<Point> positions)
{
	// initialize point features
	m_point_features.clear();

	for(int i = 0; i < names.size(); ++i)
	{
		m_point_features.push_back(point_feature_t(names[i], positions[i]));
	}

	InitialzeLabels();

	CreateScreenGrid();

	AddConflictParticles(positions); // add all point features to conflict particles

	// initialize space sampling function parameters
	m_param.setParams();

	m_computed = false;

	Point p(-m_screen_width/2. + m_cell_width, m_screen_height/2. - m_cell_height);
	BBox box(p, p + Point(50, -20));
	label_t la("saf", p, box);

	LabelCoverCell(la);
}

//
// positions if the positions of all point-features
// we initialize the label on the position of each point-feature
//
void LabelManager::InitialzeLabels()
{

	m_max_label_height = 12;
	m_min_label_height = 4;
	
	m_labels.clear();

	for(int i = 0; i < m_point_features.size(); ++i)
	{
		Point &p = m_point_features[i].pos;

		// calculate the label's bounding box
		int len = GetBitmapStringLength(m_point_features[i].name);
		BBox box(p, p + Point(len, -m_max_label_height)); 
		label_t label(m_point_features[i].name, p, box);

		m_labels.push_back(label);
	}
}

void LabelManager::CreateScreenGrid()
{
	m_grid.clear();

	CalcMaxMinLabelLength();

	m_cell_width = int(m_max_label_width + 0.5);

	m_cell_height = m_max_label_height;

	int cols = int(m_screen_width * 1.0 / (m_cell_width * 1.0) + 0.5);
	int rows = int(m_screen_height * 1.0 / (m_cell_height * 1.0) + 0.5);

	m_grid.resize(rows);

	for(int  x = 0; x < rows; ++x)
	{
		for(int y = 0; y < cols; ++y)
		{
			m_grid[x].push_back(grid_elem_t(x, y));
		}
	}
}

int LabelManager::GetBitmapStringLength(std::string str)
{
	int len;
	unsigned char *cstr = new unsigned char[str.length() + 1];
	int si;
	for(si=0; si<str.length(); si++)
		cstr[si]=str[si];

	cstr[si]='\0';
	len = glutBitmapLength(GLUT_BITMAP_HELVETICA_10, cstr);
	delete cstr;
	return len;
}

void LabelManager::CalcMaxMinLabelLength()
{
	m_max_label_width = -1;
	m_min_label_width = FLT_MAX;

	for(int i = 0; i < m_labels.size(); ++i)
	{
		float len = m_labels[i].box.GetWidth();

		if(m_max_label_width < len)  m_max_label_width = len;
		if(m_min_label_width > len)  m_min_label_width = len;
	}
}

void LabelManager:: GetCellIndex(Point p, int &row, int &col)
{
	//float x= p[0] + m_screen_width/2.;
	//float y= m_screen_height - (p[1] + m_screen_height/2.);
	float x= p[0];
	float y= m_screen_height - (p[1]);
	col = int(x / m_cell_width);
	row = int(y / m_cell_height);
}

void LabelManager::LabelCoverCell(const label_t &label)  // judge which cell is covered by the label
{
	// if not initialize yet
	if(m_grid_covered.empty())
	{
		m_grid_covered.resize(m_grid.size());
		for(int i = 0; i < m_grid_covered.size(); ++i)
			m_grid_covered[i].resize(m_grid[i].size());
	}

	for(int i = 0; i < m_grid.size(); ++i)
	{
		for(int j = 0; j < m_grid[i].size(); ++j)
		{
			m_grid_covered[i][j] = false;

			BBox cell(Point(j * m_cell_width - m_screen_width/2., -i * m_cell_height + m_screen_height/2.), 
					  Point( (j+1) * m_cell_width - m_screen_width/2., -(i+1) * m_cell_height + m_screen_height/2.) );


			float dx, dy;
			if(cell.Intersect(label.box, dx, dy))
			{
				m_grid_covered[i][j] = true;
			}

		}
	}
}

bool LabelManager::ConflictTest(const label_t &label) // check if conflict happens with current label placement
{
	LabelCoverCell(label);

	//
	// for the rectangular label area
	//
	float x_left = label.box.lt[0];
	float y_bottom = label.box.rb[1];
	float x_right = label.box.rb[0];
	float y_top = label.box.lt[1];

	bool conflict = false;

	//
	// for every cell of the grid
	//
	for(int i = 0; i < m_grid.size(); ++i)
	{
		for(int j = 0; j < m_grid[i].size(); ++j)
		{
			if(m_grid_covered[i][j])
			{
				CONFLICT_PARS &particles = m_grid[i][j].particles;

				for(int k = 0; k < particles.size(); ++k) // for all particles in this cell
				{
					Point &p = particles[k].pos;

					// if the conflict point is in the area covered by the label
					if(p[0] >= x_left && p[0] <= x_right && p[1]>= y_bottom && p[1]<= y_top)
					{
						conflict = true;
						goto end;
					}
				}
			}
		}
	}

	end:
	return conflict;
}

//
// this function only works for rectangular label area
//
void LabelManager::GenerateNewConflictParticlesForLabel(label_t &label)
{
	Point &p = label.box.lt;
	float width = label.box.GetWidth();
	float height = label.box.GetHeight();

	for(int y = p[1]; y >= label.box.rb[1]; y -= m_min_label_height)
	{
		for(int x = p[0]; x <= label.box.rb[0]; x += m_min_label_width)
		{
		    Point p = Point(x,y);
			if(OutsideScreen(p))
				continue;
			AddConflictParticle(Point(x, y));
		}
	}

}

//                      |---------------------------|
//                      |      2      |       1     |
//                      |-------------o-------------|
//                      |      3      |       4     |
//                      |---------------------------|
bool LabelManager::CheckFourPositionModel(const Point &pf, label_t &label)
{
	label_t tmp_label = label;
// 1
	tmp_label.setPos(pf + Point(0, m_max_label_height+1));
	if(!ConflictTest(tmp_label))
	{
		label = tmp_label;
		return true;
	}

	// 2
	tmp_label.setPos(pf + Point(-m_max_label_width, m_max_label_height+1));
	if(!ConflictTest(tmp_label))
	{
		label = tmp_label;
		return true;
	}
	// 3
	tmp_label.setPos(pf + Point(-m_max_label_width, -1));
	if(!ConflictTest(tmp_label))
	{
		label = tmp_label;
		return true;
	}

	// 4
	tmp_label.setPos(pf + Point(0, -1));
	if(!ConflictTest(tmp_label))
	{
		label = tmp_label;
		return true;
	}

	
	return false;
}


//                |-----------------------|
//                |          6            |
//        |-------|----------|------------|-------|
//        |    7  |----------o------------| 5     |   
//        |-------|----------|------------|-------|
//                |          8            |
// 			      |-----------------------|
//
bool LabelManager::CheckEightPositionModel(const Point &pf, label_t &label)
{
	label_t tmp_label = label;
	// 5
	tmp_label.setPos(pf + Point(1, m_max_label_height / 2.));
	if(!ConflictTest(tmp_label))
	{
		label = tmp_label;
		return true;
	}


	// 6
	tmp_label.setPos( pf + Point(-m_max_label_width / 2., m_max_label_height + 1) );
	if(!ConflictTest(tmp_label))
	{
		label = tmp_label;
		return true;
	}

	// 7
	tmp_label.setPos( pf + Point(-m_max_label_width - 1, m_max_label_height / 2.));
	if(!ConflictTest(tmp_label))
	{
		label = tmp_label;
		return true;
	}
// 8
	tmp_label.setPos( pf + Point(-m_max_label_width / 2., -1));
	if(!ConflictTest(tmp_label))
	{
		label = tmp_label;
		return true;
	}
	
	return false;
}


bool LabelManager::CheckDistantPosition(const Point &pf, label_t &label)
{
	label_t tmp = label;
	for(int m = 1; m <= m_param.mmax; ++m)
	{
		Point p = SpiralFunc(m) + pf;
		if(OutsideScreen(p))
			continue;
		tmp.setPos(p);
		if(!ConflictTest(tmp))
		{
			label = tmp;
			return true;
		}
	}

	return false;
}

Point LabelManager::SpiralFunc(float m)
{
	Point p;
	p[0] = m_param.d * cosf( 2*M_PI * sqrtf( m / m_param.mmax) * m_param.c );
	p[1]= sinf( 2 * M_PI * sqrtf( m / m_param.mmax) * m_param.c);

	p = p * (m / m_param.mmax) * m_param.r;

	return p;
}

void LabelManager::Labeling()
{
	for(int i = 0 ; i < m_point_features.size(); ++i) // for each point-feature
	{
		Point &pf = m_point_features[i].pos;
		label_t &label = m_labels[i];

		if(CheckFourPositionModel(pf, label))
		{
			GenerateNewConflictParticlesForLabel(label);
			continue;
		}

		if(CheckEightPositionModel(pf, label))
		{
			GenerateNewConflictParticlesForLabel(label);
			continue;
		}

		if(CheckDistantPosition(pf, label))
 		{
 			GenerateNewConflictParticlesForLabel(label);
 			continue;
 		}

		// if can't find a labeling position, do not show this label
		label.show = false; 
	}

	m_computed = true;
}

void LabelManager::Render()
{
	if(!m_computed)
	{
		Labeling();
		m_computed = true;
	}

	//RenderConflictParticles();

	for(int i = 0; i < m_labels.size(); ++i)
	{
			if( !m_labels[i].show )
				continue; 

		Point &pos = m_labels[i].pos;
		pos[2] = 0;
		std::string &name = m_labels[i].str;
		Color &color = m_labels[i].color;

		RenderLines(m_point_features[i].pos, m_labels[i]);


		//Draw text
		char* ss = new char[name.length()+2];
		int si;
		for(si=0; si<name.length(); si++)
		{
			ss[si]=name[si];
		}
		ss[si+1]='\0';


		char* p; // Temp pointer to the string varible passed in

		// using the function glutBitmapCharacter to render the font
		// the raster pos is at the left bottom corner of the text
		// but out pos of label is at the left top corner
		// so we need a transform
		//Point raster_pos = pos + Point(0, -m_max_label_height);
		Point raster_pos = pos;

		glDisable(GL_LIGHTING);
		glColor4ub(color.r, color.g, color.b, 180);
		// Specify the initial position on the screen where the text
		// will be diaplayed

		glRasterPos2f(raster_pos[0], raster_pos[1]);
		//glRasterPos2f(2*raster_pos[0]/m_screen_width-1, 2*raster_pos[1]/m_screen_height-1);
		//glRasterPos2f(-_wnd_width/2 + cc*_cell_width, _wnd_height/2 - cr * _cell_height);
		glDisable(GL_TEXTURE_2D);
		// Parse the string passed in and display to screen one char at a time
		for (p = ss; *p; p++)
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12 , *p); 

		delete ss;
	}

	//RenderSpaceFunc(m_point_features[0].pos);
}

void LabelManager::RenderLines(Point &feature_pos, label_t &label)
{
	Point center = label.pos;// + Point( label.box.GetWidth() / 2., label.box.GetHeight() / 2. );
	center[2] = 0;
// 	if(center[0] > feature_pos.x)
// 	{
// 		if(center[1]> feature_pos[1])
// 		{
// 			DrawLine(feature_pos, Point( label.box.lt[0], label.box.rb[1]));
// 		}
// 		else
// 		{
// 			DrawLine(feature_pos, label.box.lt);
// 		}
// 	}
// 	else
// 	{
// 		if(center[1]> feature_pos[1])
// 		{
// 			DrawLine(feature_pos, label.box.rb);
// 		}
// 		else
// 		{
// 			DrawLine(feature_pos, Point( label.box.rb[0], label.box.lt[1]));
// 		}
// 	}

	DrawLine(feature_pos, center);
}

void LabelManager::DrawLine(Point &p1, Point &p2, Color c)
{
	
	glLineWidth(1.);
	glBegin(GL_LINES);
	glColor4ub(c.r, c.g, c.b, c.a);
	glVertex3f(p1[0], p1[1], p1[2]);
	glVertex3f(p2[0], p2[1], p2[2]);
	glEnd();
}

bool LabelManager::OutsideScreen(Point &p)
{
	if(p[0] <= -m_screen_width/2. || p[0] >= m_screen_width/2.
		|| p[1]>= m_screen_height/2. || p[1]<= -m_screen_height/2.)
		return true;

	return false;
}

//
// for debug
//
void LabelManager::RenderConflictParticles()
{
	for(int i = 0; i < m_grid.size(); ++i)
	{
		for(int j = 0; j < m_grid[i].size(); ++j)
		{
			CONFLICT_PARS &particles = m_grid[i][j].particles;

			for(int k = 0; k < particles.size(); ++k)
			{glPointSize(1.5);
				glBegin(GL_POINTS);
				
				glColor4ub(255,0,0,255);
				glVertex2f(particles[k].pos[0], particles[k].pos[1]);
				glEnd();
			}
		}
	}
}

void LabelManager::RenderSpaceFunc(Point &pf)
{
	std::vector<Point> points;
	for(int m = 1; m <= m_param.mmax; ++m)
	{
		Point p = SpiralFunc(m) + pf;
		points.push_back(p);
	}

	for(int i = 0; i < points.size(); ++i)
	{
		glPointSize(2.);
		glBegin(GL_POINTS);
		glColor4ub(0,255,0,255);
		glVertex2f(points[i][0], points[i][1]);
		glEnd();
	}
}
