#include "MySimple2DGrid.h"
#include <iostream>
using namespace std;

MySimple2DGrid::MySimple2DGrid(void)
{
}


MySimple2DGrid::~MySimple2DGrid(void)
{
}

void MySimple2DGrid::SetRange(float startx, float endx, float starty, float endy){
	mStart[0] = startx;
	mStart[1] = starty;
	mEnd[0] = endx;
	mEnd[1] = endy;
}
void MySimple2DGrid::SetRange(const MyVec2f& start, const MyVec2f& end){
	mStart = start;
	mEnd = end;
}
void MySimple2DGrid::SetXDivision(int n){
	mDivisionX = n;
}
void MySimple2DGrid::SetYDivision(int n){
	mDivisionY = n;
}

int MySimple2DGrid::GetStatusByte(MyVec2f pos) const{
	int byte = 0;
	if(pos[0]<mStart[0]) byte |= LEFT;
	if(pos[0]>mEnd[0]) byte |= RIGHT;
	if(pos[1]<mStart[1]) byte |= BOTTOM;
	if(pos[1]>mEnd[1]) byte |= TOP;
	return byte;
}

bool MySimple2DGrid::IsIndexValid(const MyVec2i& index) const{
	if(index[0]<0) return false;
	if(index[1]<0) return false;
	if(index[0]>mDivisionX-1) return false;
	if(index[1]>mDivisionY-1) return false;
	return true;
}

MyVec2i MySimple2DGrid::GetGridIndex(MyVec2f pos) const{
	MyVec2f diff = pos-mStart;
	float cellWidth = (mEnd[0]-mStart[0])/mDivisionX;
	float cellHeight = (mEnd[1]-mStart[1])/mDivisionY;
	int x;
	if(pos[0] == mEnd[0]){
		x = mDivisionX-1;
	}
	else{
		x = diff[0]/cellWidth;
	}
	int y;
	if(pos[1] == mEnd[1]){
		y = mDivisionY-1;
	}
	else{
		y = diff[1]/cellHeight;
	}
	return MyVec2i(x,y);
}

MyVec2i MySimple2DGrid::ClampIndex(const MyVec2i& idx) const{
	MyVec2i id = idx;
	if(id[0]<0){
		id[0] = 0;
	}
	else if(id[0] > mDivisionX-1) {
		id[0] = mDivisionX-1;
	}
	if(id[1]<0) {
		id[1] = 0;
	}
	else if(id[1] > mDivisionY-1) {
		id[1] = mDivisionY-1;
	}
	return id;
}

MyArray2i* MySimple2DGrid::MakePassGridIndexArray(MyVec2f start, MyVec2f end) const{
	/*
	int startByte = this->GetStatusByte(start);
	int endByte = this->GetStatusByte(end);

	// guarantee no intersection
	if(startByte & endByte != 0){
		return 0;
	}
	
	MyArray2i* rst = new MyArray2i;
	// get the line y = a*x+b
	if(start[0] == end[0]){
		MyVec2i index = this->GetGridIndex(MyVec2f(start[0], mStart[1]));
		for(int i = 0;i<mDivisionY;i++){
			rst->push_back(MyVec2i(index[0], i));
		}
		return rst;
	}
	if(start[1] == end[1]){
		MyVec2i index = this->GetGridIndex(MyVec2f(mStart[0], start[1]));
		for(int i = 0;i<mDivisionX;i++){
			rst->push_back(MyVec2i(i, index[1]));
		}
		return rst;
	}
	//MyVec2f _start = (start[0]<end[0]?start:end);
	//MyVec2f _end = (start[0]>=end[0]?start:end);
	// a small value
	float cellWidth = (mEnd[0]-mStart[0])/mDivisionX;
	float cellHeight = (mEnd[1]-mStart[1])/mDivisionY;
	float cellRatio = cellHeight/cellWidth;
	float lineRatio = (end[1]-start[1])/(end[0]-start[0]);

	// line is less stepy, more x than y
	if(cellRatio>fabs(lineRatio)){
		float esp = cellWidth/10000;
		float a = (end[1]-start[1])/(end[0]-start[0]);
		float b = start[1]-a*start[0];

		// get the start most possible intersection
		float yStart = a*mStart[0]+b;
		MyVec2f startPossibleInt(mStart[0], yStart);
		float yStep = lineRatio*cellWidth;
		for(int i = 0;i<mDivisionX;i++){
			MyVec2i index = this->GetGridIndex(MyVec2f(startPossibleInt[0]+i*cellWidth, 
				startPossibleInt[1]+i*yStep));
			if(this->IsIndexValid(index)){
				rst->push_back(index);
			}
		}
		return rst;
	}
	// line is more stepy, more y than x
	else{
		float esp = cellHeight/10000;
		float a = (end[1]-start[1])/(end[0]-start[0]);
		float b = start[1]-a*start[0];

		// get the start most possible intersection
		float xStart = (mStart[1]-b)/a;
		MyVec2f startPossibleInt(xStart, mStart[1]);
		float xStep = cellHeight/lineRatio;

		for(int i = 0;i<mDivisionY;i++){
			MyVec2i index = this->GetGridIndex(MyVec2f(startPossibleInt[0]+i*xStep, 
				startPossibleInt[1]+i*cellHeight));
			if(this->IsIndexValid(index)){
				rst->push_back(index);
			}
		}
		return rst;
	}
	*/
	MyArray2i* rst = new MyArray2i;
	MyVec2f inStart, inEnd;
	bool hasIn = this->CohenSutherlandLineClip(start,end,inStart,inEnd);
	if(!hasIn){
		return rst;
	}
	else{
		float a;
		if(inEnd[0] == inStart[0]){
			a = 9999999999999999.f;
		}
		else a = (inEnd[1]-inStart[1])/(inEnd[0]-inStart[0]);
		float b = inStart[1]-a*inStart[0];
		float cellRatio = this->GetCellHeight()/this->GetCellWidth();

		MyVec2f offset;
		if(fabs(a)<cellRatio){
			offset[0] = this->GetCellWidth();
			offset[1] = this->GetCellWidth()*a;
		}
		else{
			offset[0] = this->GetCellHeight()/a;
			offset[1] = this->GetCellHeight();
		}
		if(offset[0]<0) offset *= -1;
		
		MyVec2f tmp = (inStart[0] < inEnd[0] ? inStart : inEnd);
		do{
			MyVec2i idx = this->GetGridIndex(tmp);
			if(rst->size()>0){
				MyVec2i lastIdx = rst->back();
				if(!MySimple2DGrid::IsCellConnected(lastIdx,idx)){
					MyVec2f mid;
					mid[0] = this->GetLowPos(idx)[0]-this->GetCellWidth()/10000;
					mid[1] = a*mid[0]+b;
					MyVec2i midIdx = this->GetGridIndex(mid);
					if(midIdx != lastIdx && midIdx != idx){
						rst->push_back(midIdx);
					}
				}
			}
			rst->push_back(idx);
			tmp += offset;
		}while( (tmp-inStart)*(tmp-inEnd)<=0);

	}
	return rst;
}

bool MySimple2DGrid::IsCellConnected(const MyVec2i& idx1, const MyVec2i& idx2, bool diagCounts){
	int xDiff = idx1[0]-idx2[0];
	int yDiff = idx1[1]-idx2[1];
	int thre = diagCounts?2:1;
	int diffSqrd = xDiff*xDiff+yDiff*yDiff;
	return diffSqrd<=thre;
}

bool MySimple2DGrid::IsCellConnected(const MyVec2i& idx1, const MyArray2i& idxArray, bool diagCounts){
	for(int i = 0;i<idxArray.size();i++){
		if(MySimple2DGrid::IsCellConnected(idx1, idxArray[i], diagCounts)){
			return true;
		}
	}
	return false;
}
bool MySimple2DGrid::IsCellConnected(const MyArray2i& idxArray1, const MyArray2i& idxArray2, bool diagCounts){
	for(int i = 0;i<idxArray1.size();i++){
		if(MySimple2DGrid::IsCellConnected(idxArray1[i], idxArray2, diagCounts)){
			return true;
		}
	}
	return false;
}


float MySimple2DGrid::GetCellWidth() const{
	return (mEnd[0]-mStart[0])/mDivisionX;

}

float MySimple2DGrid::GetCellHeight() const{
	return (mEnd[1]-mStart[1])/mDivisionX;
}

MyVec2f MySimple2DGrid::GetCellSize() const{
	return MyVec2f(this->GetCellWidth(), this->GetCellHeight());
}
MyVec2f MySimple2DGrid::GetLowPos(const MyVec2i& idx) const{
	return mStart + MyVec2f(idx[0]*this->GetCellWidth(), idx[1]*this->GetCellHeight());
}

/*
MyArray2i* MySimple2DGrid::MakeCellsAround(const MyVec2f& center, float maxDistance) const{
	MyArray2i* rst = new MyArray2i;
	MyVec2i centerCell = this->GetGridIndex(center);

	return rst;
}
*/
MyArray2f* MySimple2DGrid::MakeContourArray(const MyArray2i& inArray, bool clockWise) const{
	MyArray2i* contourCellIdx = MySimple2DGrid::MakeContourCellIndexArray(inArray);
	MyArray2i* contourIndex = MySimple2DGrid::MakeContourIndexArray(*contourCellIdx);
	MyArray2f* rst = new MyArray2f;
	for(int i = 0;i<contourIndex->size();i++){
		rst->push_back(this->GetLowPos(contourIndex->at(i)));
	}
	delete contourCellIdx;
	delete contourIndex;
	return rst;
}

MyArray2i* MySimple2DGrid::MakeBoxCollidingCells(const MyVec2f& low, const MyVec2f high) const{
	MyArray2i* rst = new MyArray2i;
	MyVec2i lowleft = this->GetGridIndex(low);
	MyVec2i topright = this->GetGridIndex(high);
	MyVec2i lowright = this->GetGridIndex(MyVec2f(high[0], low[1]));
	MyVec2i topleft = this->GetGridIndex(MyVec2f(low[0], high[1]));
	if(!this->IsIndexValid(lowleft)
		&& !this->IsIndexValid(topright)
		&& !this->IsIndexValid(lowright)
		&& !this->IsIndexValid(topleft)){
		// not intersection at all
		return rst;
	}
	
	MyVec2i lowleftClamp = this->ClampIndex(lowleft);
	MyVec2i toprightClamp = this->ClampIndex(topright);
	MyVec2i lowrightClamp = this->ClampIndex(lowright);
	MyVec2i topleftClamp = this->ClampIndex(topleft);

	for(int i = lowleftClamp[0]; i <= lowrightClamp[0]; i++){
		for(int j = lowleftClamp[1]; j <= toprightClamp[1]; j++){
			rst->push_back(MyVec2i(i,j));
		}
	}
	return rst;
}

bool MySimple2DGrid::CohenSutherlandLineClip(const MyVec2f& start, const MyVec2f& end,
	MyVec2f& outStart, MyVec2f& outEnd) const{

	int outcode0 = this->GetStatusByte(start);
	int outcode1 = this->GetStatusByte(end);
	bool accept = false;


	float x0 = start[0];
	float x1 = end[0];
	float y0 = start[1];
	float y1 = end[1];
	float xmin = mStart[0];
	float xmax = mEnd[0];
	float ymin = mStart[1];
	float ymax = mEnd[1];

	while (true) {
		if (!(outcode0 | outcode1)) { // Bitwise OR is 0. Trivially accept and get out of loop
			accept = true;
			break;
		} else if (outcode0 & outcode1) { // Bitwise AND is not 0. Trivially reject and get out of loop
			break;
                } else {
			// failed both tests, so calculate the line segment to clip
			// from an outside point to an intersection with clip edge
			double x, y;
 
			// At least one endpoint is outside the clip rectangle; pick it.
			int outcodeOut = outcode0 ? outcode0 : outcode1;
 
			// Now find the intersection point;
			// use formulas y = y0 + slope * (x - x0), x = x0 + (1 / slope) * (y - y0)
			if (outcodeOut & TOP) {           // point is above the clip rectangle
				x = x0 + (x1 - x0) * (ymax - y0) / (y1 - y0);
				y = ymax;
			} else if (outcodeOut & BOTTOM) { // point is below the clip rectangle
				x = x0 + (x1 - x0) * (ymin - y0) / (y1 - y0);
				y = ymin;
			} else if (outcodeOut & RIGHT) {  // point is to the right of clip rectangle
				y = y0 + (y1 - y0) * (xmax - x0) / (x1 - x0);
				x = xmax;
			} else if (outcodeOut & LEFT) {   // point is to the left of clip rectangle
				y = y0 + (y1 - y0) * (xmin - x0) / (x1 - x0);
				x = xmin;
			}
 
			// Now we move outside point to intersection point to clip
			// and get ready for next pass.
			if (outcodeOut == outcode0) {
				x0 = x;
				y0 = y;
				outcode0 = this->GetStatusByte(MyVec2f(x0, y0));
			} else {
				x1 = x;
				y1 = y;
				outcode1 = this->GetStatusByte(MyVec2f(x1, y1));
			}
		}
	}
	if (accept) {
		outStart = MyVec2f(x0,y0);
		outEnd = MyVec2f(x1,y1);
		return true;
	}
	else return false;
}

MyArray2i* MySimple2DGrid::MakeContourCellIndexArray(const MyArray2i& inArray, bool diagCounts){
	MyArray2i* rst = new MyArray2i;
	// if a cell is surrounded by 4 directions
	// it is not a boundary
	int surrondingByte = LEFT + RIGHT + TOP + BOTTOM;
	// if diagonal counts
	// it must be surrounded by 8 directions
	// the byte will be multiplied by 3
	if(diagCounts) surrondingByte *= 3;
	for(int i = 0;i<inArray.size();i++){
		const MyVec2i& subject = inArray[i];
		int byte = 0;
		for(int j = 0;j<inArray.size();j++){
			if(i!=j){
				const MyVec2i& test = inArray[j];
				if(MySimple2DGrid::IsCellConnected(test, subject, diagCounts)){
					byte += MySimple2DGrid::GetConnectionByte(test, subject);
					if(byte == surrondingByte){
						break;
					}
				}
			}
		}
		if(byte != surrondingByte){
			rst->push_back(subject);
		}
	}
	return rst;
}


MyArray2i* MySimple2DGrid::MakeContourIndexArray(const MyArray2i& inContourArray, bool clockWise){
	MyArray2i* contourSegArray = new MyArray2i;
	for(int i = 0;i<inContourArray.size();i++){
		int byte = MySimple2DGrid::GetConnectionByte(inContourArray[i], inContourArray);
		MyArray2i* contourSeg = MySimple2DGrid::MakeContourIndexArray(inContourArray[i],byte, clockWise);
		contourSegArray->PushBack(contourSeg);
		delete contourSeg;
	}

	MyArray2i* rst = new MyArray2i;
	MyVec2i end;
	if(contourSegArray->size()>=2){
		rst->push_back(contourSegArray->at(0));
		end = contourSegArray->at(1);
		contourSegArray->erase(contourSegArray->begin(), contourSegArray->begin()+2);
	
		bool findEnd = true;
		while(rst->back() != end){
			findEnd = false;
			for(int i = 0;i<contourSegArray->size();i+=2){
				if(contourSegArray->at(i) == end){
					rst->push_back(end);
					end = contourSegArray->at(i+1);
					contourSegArray->erase(contourSegArray->begin()+i, contourSegArray->begin()+2+i);
					findEnd = true;
				}
			}

			if(contourSegArray->empty()){
				if(MySimple2DGrid::IsCellConnected(rst->front(),rst->back())){
					// job done!
					break;
				}
			}
			if(!findEnd){
				if(!(MySimple2DGrid::IsCellConnected(rst->front(),rst->back()))){
					// something wrong here
					cout << "wrong in finding contour!!!\n";
				}
				break;
			}
		}
	}
	delete contourSegArray;
	return rst;
}

MyArray2i* MySimple2DGrid::MakeContourIndexArray(const MyVec2i& idx, int neighborByte, bool clockWise){
	MyArray2i* rst = new MyArray2i;
	if((neighborByte & LEFT) == 0){
		rst->push_back(idx);
		rst->push_back(idx+MyVec2i(0,1));
	}
	
	if((neighborByte & TOP) == 0){
		rst->push_back(idx+MyVec2i(0,1));
		rst->push_back(idx+MyVec2i(1,1));
	}

	if((neighborByte & RIGHT) == 0){
		rst->push_back(idx+MyVec2i(1,1));
		rst->push_back(idx+MyVec2i(1,0));
	}
	
	if((neighborByte & BOTTOM) == 0){
		rst->push_back(idx+MyVec2i(1,0));
		rst->push_back(idx);
	}

	if(!clockWise){
		for(int i = 0;i<rst->size()/2;i++){
			rst->Swap(i, rst->size()-i);
		}
	}

	return rst;
}


int MySimple2DGrid::GetConnectionByte(const MyVec2i& source, const MyVec2i& target){
	int byte = 0;
	if(source[0] < target[0]) byte |= LEFT;
	if(source[0] > target[0]) byte |= RIGHT;
	if(source[1] < target[1]) byte |= BOTTOM;
	if(source[1] > target[1]) byte |= TOP;
	return byte;
}

int MySimple2DGrid::GetConnectionByte(const MyVec2i& idx, const MyArray2i& idxArray){
	int byte = 0;
	for(int i = 0;i<idxArray.size();i++){
		const MyVec2i& test = idxArray[i];
		if(idx!=test){
			if(MySimple2DGrid::IsCellConnected(test, idx)){
				byte += MySimple2DGrid::GetConnectionByte(test, idx);
			}
		}
	}
	return byte;
}