#include "MyVisualElement.h"
#include "MyPrimitiveDrawer.h"
#include "MyUtility.h"
#include "MyColorConverter.h"
#include "VisConfig.h"
#include <cmath>

#include "MyTextureHelper.h"

#define NUMRANDS 1389
#define HIGHLIGHTLINEWIDTH 1

#define CIRCLELEVEL 10

// 
#define MAXLENGTH 3

//#include <fstream>
//using namespace std;

int MyVisualElementSingleValueMapper::mBackground = 0;

MyString MyVis::GetVisualElementName(const MyVisualElement& ele){
	switch(ele){
	case LENGTH:
		return "Length";
		break;
	case SIZE:
		return "Size";
		break;
	case ANGLE:
		return "Angle";
		break;
	case SATURATION:
		return "Lightness";
		break;
	case HUE:
		return "Hue";
		break;
	case TEXTURE:
		return "Texture";
		break;
	case DENSITY:
		return "Density";
		break;
	case SHAPE:
		return "Shape";
		break;
	case SLOPE:
		return "Slope";
		break;
	case NONE:
	default:
		return "None";
		break;

	}
}

MyVisualElementSingleValueMapper::MyVisualElementSingleValueMapper(){
	MyArray<MyColor4f> hues;
	MyArray<MyColor4f> sats;
	for (int i = 0; i <= 20; i++){
		float value = i/20.f;
		//rgba rgb = MyColorConverter::lab2rgb(i/20.f * 100, 1, 1);
		MyColor4f hue = MyColorConverter::lab2rgb(100, 
			cos(value*MY_PI)*500, sin(value*MY_PI)*200);
		hue.a = 1.f;
		//hues << hue;
		MyColor4f sat = MyColorConverter::lab2rgb(value*100, 0, 0);
		sat.a = 1.f;
		sats << sat;
	}
	/*
		http://www.cs.utah.edu/~gk/papers/vis02/FaceLumin.pdf*
		gama correlation needed
		interpolation need modification*/
	hues << MyColor4f(0.847,0.057,0.057)
		<< MyColor4f(0.527,0.527,0.000)
		<< MyColor4f(0.000,0.592,0.000)
		<< MyColor4f(0.000,0.559,0.559)
		<< MyColor4f(0.316,0.316,0.991)
		<< MyColor4f(0.718,0.000,0.718);
		//<< MyColor4f(0.847,0.057,0.057);
	
	for(int i = 0;i<20;i++){
		float value = i/20.f;
		//hues << MyColorConverter::lab2rgb(50,64*cos(value*2*MY_PI),64*cos(value*2*MY_PI));
	}
	mHueScale.SetDomain(0,1);
	mHueScale.SetRange(hues);
	mHueScale.GetRange().SetInterpolationMethod(My1DSampler<MyColor4f>::INTERPOLATE_GAMMA);
	mHueScale.GetRange().SetGamma(2.68);
	mSaturationScale.SetDomain(0,1);
	mSaturationScale.SetRange(sats);
	mTextureHelper.GenerateTexture();
	mHighlightTextureHelper.GenerateTexture();

	/*
	char space = ' ';
	ofstream hueFile("hueScale.txt");
	for (int i = 0; i <= 32; i++){
		float value = i / 32.f;
		auto c = mHueScale[value];
		hueFile << c.r << space << c.g << space << c.b << endl;
	}
	hueFile.close();
	ofstream satFile("satScale.txt");
	for (int i = 0; i <= 32; i++){
		float value = i / 32.f;
		auto c = mSaturationScale[value];
		satFile << c.r << space << c.g << space << c.b << endl;
	}
	satFile.close();
	*/

	mRands.reserve(NUMRANDS);
	for(int i = 0;i<NUMRANDS;i++){
		mRands << rand();
	}

	mHighlightMode = false;
	mHighlightColor = MyColor4f(1, 0, 0, 1);

	// calculate circular sample array
	int sumSteps = 0;
	for(int i = 0;i<CIRCLELEVEL;i++){
		sumSteps += i;
	}
	float samplePerStep = DENSITY_MAX_DOTS/(float)sumSteps;
	MyArrayf circleCulmulatedsamples(1,0);
	for(int i = 1;i<CIRCLELEVEL+1;i++){
		circleCulmulatedsamples << circleCulmulatedsamples[i-1]+i*samplePerStep;
	}
	mCircularSampleArray.clear();
	mCircularSampleArray.reserve(DENSITY_MAX_DOTS);
	for(int i = 0;i<CIRCLELEVEL;i++){
		int numSamplesInStep = circleCulmulatedsamples[i+1]-circleCulmulatedsamples[i];
		for(int j = 0;j<numSamplesInStep;j++){
			mCircularSampleArray << MyVec2f((i+rand()%100/(float)100)/(float)(CIRCLELEVEL),j/(float)numSamplesInStep*2*MY_PI);
		}
	}

	mHighlightBoarder = 1;
}

MyVisualElementSingleValueMapper::~MyVisualElementSingleValueMapper(){
	mTextureHelper.DeleteTexture();
	mHighlightTextureHelper.DeleteTexture();
}

void MyVisualElementSingleValueMapper::SetScale(MyScale<float, float> scale){
	mScale = scale;
}

void MyVisualElementSingleValueMapper::SetVisualElement(MyVis::MyVisualElement ele){
	mVisualElement = ele;
}

void MyVisualElementSingleValueMapper::DrawValueMarker(float value) const{
	//this->DrawShapeMarker(value);
	//this->DrawTextureMarker(value);
	//DrawAngleMarker(value);
	//return;

	switch (mVisualElement){
		case MyVis::NONE:
			this->DrawDefaultMarker(0.5);
			break;
		case MyVis::LENGTH:
			this->DrawLengthMarker(value);
			break;
		case MyVis::SIZE:
			this->DrawSizeMarker(value);
			break;
		case MyVis::ANGLE:
			this->DrawAngleMarker(value);
			break;
		case MyVis::SATURATION:
			this->DrawSaturationMarker(value);
			break;
		case MyVis::HUE:
			this->DrawHueMarker(value);
			break;
		case MyVis::TEXTURE:
			this->DrawTextureMarker(value);
			break;
		case MyVis::DENSITY:
			this->DrawDensityMarker(value);
			break;
		case MyVis::SHAPE:
			this->DrawShapeMarker(value);
			break;
		case MyVis::SLOPE:
			this->DrawSlopeMarker(value);
			break;
		default:
			MyPrimitiveDrawer::Color(MyColor4f::white());
			MyPrimitiveDrawer::Sphere(0.5f);
			break;
	}

}

void MyVisualElementSingleValueMapper::DrawDefaultMarker(float value) const{
	float radius = value;
	//MyPrimitiveDrawer::DrawSphereAt(MyVec3f::zero(), radius);
	if (mBackground == 0) MyPrimitiveDrawer::Color(MyColor4f::black());
	else MyPrimitiveDrawer::Color(MyColor4f::white());
	MyPrimitiveDrawer::BeginTriangleFan();
	MyPrimitiveDrawer::Vertex(MyVec3f::zero());
	for (int ip = 0; ip <= 40; ip++){
		float t_angle = ip / 40.f*MY_PI * 2;
		MyPrimitiveDrawer::Vertex(radius*MyVec3f(cos(t_angle), sin(t_angle), 0));
	}
	MyPrimitiveDrawer::EndPrimitive();

	if (mBackground == 0) MyPrimitiveDrawer::Color(MyColor4f::black());
	else MyPrimitiveDrawer::Color(MyColor4f::white());
	MyPrimitiveDrawer::DrawCircle(MyVec3f::zero(), radius);
	MyPrimitiveDrawer::SetLineWidth(1);
}

void MyVisualElementSingleValueMapper::DrawLengthMarker(float value) const{
	float length = mScale[value]*MAXLENGTH;
	float halfWidth = 0.18;
	MyArray3f vertices;
	vertices << MyVec3f(-halfWidth, 0.f, 0.f) << MyVec3f(halfWidth, 0.f, 0.f)
		<< MyVec3f(halfWidth, length, 0.f) << MyVec3f(-halfWidth, length, 0.f);
	if (mBackground == 0) MyPrimitiveDrawer::Color(MyColor4f::black());
	else MyPrimitiveDrawer::Color(MyColor4f::white());
	MyPrimitiveDrawer::BeginQuads();
	MyPrimitiveDrawer::Vertices(vertices);
	MyPrimitiveDrawer::EndPrimitive();

	if (mBackground == 0) MyPrimitiveDrawer::Color(MyColor4f::white());
	else MyPrimitiveDrawer::Color(MyColor4f::black());
	MyPrimitiveDrawer::WirePolygon();
	MyPrimitiveDrawer::BeginQuads();
	MyPrimitiveDrawer::Vertices(vertices);
	MyPrimitiveDrawer::EndPrimitive();
	MyPrimitiveDrawer::FillPolygon();
	MyPrimitiveDrawer::SetLineWidth(1);
}

void MyVisualElementSingleValueMapper::DrawSizeMarker(float value) const{
	float radius = sqrtf(mScale[value])*0.5+AREA_OFFSET;
	//MyPrimitiveDrawer::DrawSphereAt(MyVec3f::zero(), radius);
	if (mBackground == 0) MyPrimitiveDrawer::Color(MyColor4f::black());
	else MyPrimitiveDrawer::Color(MyColor4f::white());
	MyPrimitiveDrawer::BeginTriangleFan();
	MyPrimitiveDrawer::Vertex(MyVec3f::zero());
	for(int ip = 0;ip<=40;ip++){
		float t_angle = ip/40.f*MY_PI*2;
		MyPrimitiveDrawer::Vertex(radius*MyVec3f(cos(t_angle),sin(t_angle),0));
	}
	MyPrimitiveDrawer::EndPrimitive();

	if (mBackground == 0) MyPrimitiveDrawer::Color(MyColor4f::white());
	else MyPrimitiveDrawer::Color(MyColor4f::black());
	MyPrimitiveDrawer::DrawCircle(MyVec3f::zero(),radius);
	MyPrimitiveDrawer::SetLineWidth(1);
}

void MyVisualElementSingleValueMapper::DrawAngleMarker(float value) const{
	float radius = 0.5f;
	if (mBackground == 0) MyPrimitiveDrawer::Color(MyColor4f::white());
	else MyPrimitiveDrawer::Color(MyColor4f::gray());
	MyPrimitiveDrawer::BeginTriangleFan();
	MyPrimitiveDrawer::Vertex(MyVec3f::zero());
	for(int ip = 0;ip<=40;ip++){
		float t_angle = ip/40.f*MY_PI*2;
		MyPrimitiveDrawer::Vertex(radius*MyVec3f(cos(t_angle),sin(t_angle),0));
	}
	MyPrimitiveDrawer::EndPrimitive();

	if (mBackground == 0) MyPrimitiveDrawer::Color(MyColor4f::black());
	else MyPrimitiveDrawer::Color(MyColor4f::white());
	MyPrimitiveDrawer::DrawCircle(MyVec3f::zero(), 0.5f);
	float angle = mScale[value]*2*MY_PI;
	MyPrimitiveDrawer::BeginTriangleFan();
	MyPrimitiveDrawer::Vertex(MyVec3f::zero());
	for(int ip = -10;ip<=10;ip++){
		float t_angle = ip/20.f*angle;
		MyPrimitiveDrawer::Vertex(radius*MyVec3f(cos(t_angle),sin(t_angle),0));
	}
	MyPrimitiveDrawer::EndPrimitive();

	if (mBackground == 0) MyPrimitiveDrawer::Color(MyColor4f::black());
	else MyPrimitiveDrawer::Color(MyColor4f::white());
	MyPrimitiveDrawer::DrawCircle(MyVec3f::zero(),radius);
	MyPrimitiveDrawer::SetLineWidth(1);
}

void MyVisualElementSingleValueMapper::DrawSlopeMarker(float value) const{
	float radius = 0.5f;
	if (mBackground == 0) MyPrimitiveDrawer::Color(MyColor4f::white());
	else MyPrimitiveDrawer::Color(MyColor4f::gray());
	MyPrimitiveDrawer::BeginTriangleFan();
	MyPrimitiveDrawer::Vertex(MyVec3f::zero());
	for(int ip = 0;ip<=40;ip++){
		float t_angle = ip/40.f*MY_PI*2;
		MyPrimitiveDrawer::Vertex(radius*MyVec3f(cos(t_angle),sin(t_angle),0));
	}
	MyPrimitiveDrawer::EndPrimitive();

	if (mBackground == 0) MyPrimitiveDrawer::Color(MyColor4f::black());
	else MyPrimitiveDrawer::Color(MyColor4f::white());
	MyPrimitiveDrawer::DrawCircle(MyVec3f::zero(), radius);
	float angle = (1-mScale[value])*MY_PI;
	MyPrimitiveDrawer::DrawLineAt(MyVec3f::zero(),radius*MyVec3f(cos(angle), sin(angle), 0));

	if (mBackground == 0) MyPrimitiveDrawer::Color(MyColor4f::black());
	else MyPrimitiveDrawer::Color(MyColor4f::white());
	MyPrimitiveDrawer::DrawCircle(MyVec3f::zero(),radius);
	MyPrimitiveDrawer::SetLineWidth(1);
}

void MyVisualElementSingleValueMapper::DrawSaturationMarker(float value) const{
	//MyColor4f color = MyColorConverter::lab2rgb(mScale[value] * 100, 1, 1);
	//MyColor4f color = MyColorConverter::lab2rgb(50, (1-mScale[value])*500, mScale[value]*200);
	//color.a = 1.0f;
	float radius = 0.5f;
	MyColor4f color = mSaturationScale[mScale[value]];
	MyPrimitiveDrawer::Color(color);
	MyPrimitiveDrawer::DrawSphereAt(MyVec3f::zero(), radius);
	//MyPrimitiveDrawer::Color(MyColor4f(1.f - color.r, 1.f - color.g, 1.f - color.b, 1.f));

	if (mBackground == 0) MyPrimitiveDrawer::Color(MyColor4f::black());
	else MyPrimitiveDrawer::Color(MyColor4f::white());
	MyPrimitiveDrawer::DrawCircle(MyVec3f::zero(), radius);

	if (mBackground == 0) MyPrimitiveDrawer::Color(MyColor4f::black());
	else MyPrimitiveDrawer::Color(MyColor4f::white());
	MyPrimitiveDrawer::DrawCircle(MyVec3f::zero(),radius);
	MyPrimitiveDrawer::SetLineWidth(1);
}

void MyVisualElementSingleValueMapper::DrawHueMarker(float value) const{
	float radius = 0.5f;
	float _value = mScale[value];
	MyColor4f color = mHueScale[_value];
	//color = MyColorConverter::lab2rgb(50,64*cos(mScale[value]*2*MY_PI),64*cos(mScale[value]*2*MY_PI));
	MyPrimitiveDrawer::Color(color);
	MyPrimitiveDrawer::DrawSphereAt(MyVec3f::zero(), radius);
	//MyPrimitiveDrawer::Color(MyColor4f(1.f - color.r, 1.f - color.g, 1.f - color.b, 1.f));

	if (mBackground == 0) MyPrimitiveDrawer::Color(MyColor4f::black());
	else MyPrimitiveDrawer::Color(MyColor4f::white());
	MyPrimitiveDrawer::DrawCircle(MyVec3f::zero(), radius);
	

	if (mBackground == 0) MyPrimitiveDrawer::Color(MyColor4f::black());
	else MyPrimitiveDrawer::Color(MyColor4f::white());
	MyPrimitiveDrawer::DrawCircle(MyVec3f::zero(),radius);
	MyPrimitiveDrawer::SetLineWidth(1);
}

void MyVisualElementSingleValueMapper::DrawTextureMarker(float value) const{
	float radius = 0.5;
	int segs = 20;
	float mValue = mScale[value];
	float freq = (mValue) * TEX_COORD_STEP + TEX_COORD_OFFSET;
	MyPrimitiveDrawer::Color(MyColor4f::white());
	mTextureHelper.ActiveTexture();
	MyPrimitiveDrawer::BeginTriangleFan();
	MyPrimitiveDrawer::TextureCoordinate(MyVec2f::zero());
	MyPrimitiveDrawer::Vertex(MyVec3f::zero());
	for(int ip = 0;ip<=10;ip++){
		float t_angle = ip/10.f*MY_PI*2;
		MyPrimitiveDrawer::TextureCoordinate(MyVec2f(cos(t_angle), sin(t_angle))*freq);
		MyPrimitiveDrawer::Vertex(radius*MyVec3f(cos(t_angle),sin(t_angle),0));
	}
	MyPrimitiveDrawer::EndPrimitive();
	mTextureHelper.DeactiveTexture();
	

	if (mBackground == 0) MyPrimitiveDrawer::Color(MyColor4f::black());
	else MyPrimitiveDrawer::Color(MyColor4f::white());
	MyPrimitiveDrawer::DrawCircle(MyVec3f::zero(),radius);
	MyPrimitiveDrawer::SetLineWidth(1);
}

void MyVisualElementSingleValueMapper::DrawDensityMarker(float value) const{
	float radius = 0.5;
	int segs = 20;
	float _v = mScale[value];
	MyPrimitiveDrawer::Color(MyColor4f::white());
	MyPrimitiveDrawer::BeginTriangleFan();
	MyPrimitiveDrawer::Vertex(MyVec3f::zero());
	for(int ip = 0;ip<=10;ip++){
		float t_angle = ip/10.f*MY_PI*2;
		MyPrimitiveDrawer::Vertex(radius*MyVec3f(cos(t_angle),sin(t_angle),0));
	}
	MyPrimitiveDrawer::EndPrimitive();

	MyPrimitiveDrawer::Color(MyColor4f::black());
	MyPrimitiveDrawer::SetPointSize(1.0);
	MyPrimitiveDrawer::BeginPoints();

	/*
	int numPoints = _v*200;
	for(int i = 0;i<numPoints;i++){
		// dont lay on the boundary
		float r = sqrtf(this->GetRand(i)%10000/10000.f)*radius*0.95;
		float phi = this->GetRand(i+1)%10000/10000.f*MY_PI*2;
		MyPrimitiveDrawer::Vertex(MyVec2f(r*cos(phi),r*sin(phi)));
	}
	*/
	int numPoints = _v*DENSITY_MAX_DOTS;
	MyArrayi seq = MyArrayi::GetSequence(0,DENSITY_MAX_DOTS-1);
	seq.Permute(mRandomSeed);
	for(int i = 0;i<numPoints;i++){
		MyVec2f pos = mCircularSampleArray[seq[i]];
		MyPrimitiveDrawer::Vertex(radius*pos[0]*MyVec2f(cos(pos[1]),sin(pos[1])));
	}
	MyPrimitiveDrawer::EndPrimitive();
	MyPrimitiveDrawer::SetPointSize(1.0);

	MyPrimitiveDrawer::Color(MyColor4f::black());
	MyPrimitiveDrawer::DrawCircle(MyVec3f::zero(),radius);
	MyPrimitiveDrawer::SetLineWidth(1);
}
/*
	https://www.wolframalpha.com/input/?i=integral%281-x%5Ea%29%5E%281%2Fa%29dx
*/

float Hypergeometric2F1(float a, float b, float c, float x){
	return 1+a*b*x/c 
		+ a*(1+a)*b*(1+b)*x*x/(2*c*(1+c))
		+ a*(1+a)*(2+a)*b*(1+b)*(2+b)*x*x*x/(6*c*(1+c)*(2+c));
}

void MyVisualElementSingleValueMapper::DrawShapeMarker(float value) const{
	//float alpha = powf(3.f,2*mScale[value]-1);
	float alpha = -log(2)/log(0.55*mScale[value]+0.3);
	float beta = alpha;
	float radius = 0.4f;
	float area = Hypergeometric2F1(-1/alpha,1/alpha,1+1/alpha,1);
	radius/=sqrtf(area);
	int segs = 180;
	MyArray2f vertices;
	vertices << MyVec2f::zero();
	
	float maxRadius = 0.8;
	for(int i = 0;i<segs;i++){
		float w = i/(float)segs*MY_PI*2;
		float x = powf(fabs(cos(w)),2/alpha);
		float y = powf(fabs(sin(w)),2/beta);
		if(i>=segs/4 && i<segs/4*3) x = -x;
		if(i>segs/2) y = -y;
		MyVec2f p = radius*MyVec2f(x,y);
		//if(p.norm() >= maxRadius) p = p.normalized()*maxRadius;
		vertices << p;
	}
	vertices << vertices[1];
	//vertices << radius*MyVec2f(maxRadius,0);
	MyPrimitiveDrawer::SetLineWidth(0.5);
	if (mBackground == 0) MyPrimitiveDrawer::Color(MyColor4f::black());
	else MyPrimitiveDrawer::Color(MyColor4f::white());
	MyPrimitiveDrawer::BeginTriangleFan();
	MyPrimitiveDrawer::Vertices(vertices);
	MyPrimitiveDrawer::EndPrimitive();

	vertices.EraseAt(0);
	if (mBackground == 0) MyPrimitiveDrawer::Color(MyColor4f::white());
	else MyPrimitiveDrawer::Color(MyColor4f::black());
	MyPrimitiveDrawer::BeginLineLoop();
	MyPrimitiveDrawer::Vertices(vertices);
	MyPrimitiveDrawer::EndPrimitive();
	
	MyPrimitiveDrawer::SetLineWidth(1);

}


void MyVisualElementSingleValueMapper::DrawLengtHighlight(float value) const{
	float length = mScale[value]*MAXLENGTH;
	float halfWidth = 0.18;
	length+=halfWidth*mHighlightBoarder;
	float newHalfWidth = halfWidth*(1+mHighlightBoarder);
	MyArray3f vertices;
	vertices << MyVec3f(-newHalfWidth, -halfWidth*mHighlightBoarder, 0.f) 
		<< MyVec3f(newHalfWidth, -halfWidth*mHighlightBoarder, 0.f)
		<< MyVec3f(newHalfWidth, length, 0.f) 
		<< MyVec3f(-newHalfWidth, length, 0.f);

	MyGraphicsTool::EnableTexture2D();
	mHighlightTextureHelper.ActiveTexture();
	MyGraphicsTool::EnableAlplaBlending();
	MyGraphicsTool::BeginQuads();
	MyGraphicsTool::TextureCoordinate(MyVec2f(0,0,0));
	MyGraphicsTool::Vertex(vertices[0]);
	MyGraphicsTool::TextureCoordinate(MyVec2f(1,0,0));
	MyGraphicsTool::Vertex(vertices[1]);
	MyGraphicsTool::TextureCoordinate(MyVec2f(1,1,0));
	MyGraphicsTool::Vertex(vertices[2]);
	MyGraphicsTool::TextureCoordinate(MyVec2f(0,1,0));
	MyGraphicsTool::Vertex(vertices[3]);
	MyGraphicsTool::EndPrimitive();
	mHighlightTextureHelper.DeactiveTexture();
	MyGraphicsTool::DisableTexture2D();
}

void MyVisualElementSingleValueMapper::DrawSizeHighlight(float value) const{
	float radius = sqrtf(mScale[value])*0.5+AREA_OFFSET;
	radius *= (1+mHighlightBoarder);
	radius += 0.2;
	MyGraphicsTool::EnableTexture2D();
	mHighlightTextureHelper.ActiveTexture();
	MyGraphicsTool::EnableAlplaBlending();
	MyGraphicsTool::BeginQuads();
	MyGraphicsTool::TextureCoordinate(MyVec2f(0,0,0));
	MyGraphicsTool::Vertex(MyVec2f(-radius,-radius,0));
	MyGraphicsTool::TextureCoordinate(MyVec2f(1,0,0));
	MyGraphicsTool::Vertex(MyVec2f(radius,-radius,0));
	MyGraphicsTool::TextureCoordinate(MyVec2f(1,1,0));
	MyGraphicsTool::Vertex(MyVec2f(radius,radius,0));
	MyGraphicsTool::TextureCoordinate(MyVec2f(0,1,0));
	MyGraphicsTool::Vertex(MyVec2f(-radius,radius,0));
	MyGraphicsTool::EndPrimitive();
	mHighlightTextureHelper.DeactiveTexture();
	MyGraphicsTool::DisableTexture2D();
}

void MyVisualElementSingleValueMapper::DrawAngleHighlight(float value) const{
	float radius = 0.5f;
	radius *= (1+mHighlightBoarder);
	MyGraphicsTool::EnableTexture2D();
	mHighlightTextureHelper.ActiveTexture();
	MyGraphicsTool::EnableAlplaBlending();
	MyGraphicsTool::BeginQuads();
	MyGraphicsTool::TextureCoordinate(MyVec2f(0,0,0));
	MyGraphicsTool::Vertex(MyVec2f(-radius,-radius,0));
	MyGraphicsTool::TextureCoordinate(MyVec2f(1,0,0));
	MyGraphicsTool::Vertex(MyVec2f(radius,-radius,0));
	MyGraphicsTool::TextureCoordinate(MyVec2f(1,1,0));
	MyGraphicsTool::Vertex(MyVec2f(radius,radius,0));
	MyGraphicsTool::TextureCoordinate(MyVec2f(0,1,0));
	MyGraphicsTool::Vertex(MyVec2f(-radius,radius,0));
	MyGraphicsTool::EndPrimitive();
	mHighlightTextureHelper.DeactiveTexture();
	MyGraphicsTool::DisableTexture2D();
}

void MyVisualElementSingleValueMapper::DrawSlopeHighlight(float value) const{
	float radius = 0.5f;
	radius *= (1+mHighlightBoarder);
	MyGraphicsTool::EnableTexture2D();
	mHighlightTextureHelper.ActiveTexture();
	MyGraphicsTool::EnableAlplaBlending();
	MyGraphicsTool::BeginQuads();
	MyGraphicsTool::TextureCoordinate(MyVec2f(0,0,0));
	MyGraphicsTool::Vertex(MyVec2f(-radius,-radius,0));
	MyGraphicsTool::TextureCoordinate(MyVec2f(1,0,0));
	MyGraphicsTool::Vertex(MyVec2f(radius,-radius,0));
	MyGraphicsTool::TextureCoordinate(MyVec2f(1,1,0));
	MyGraphicsTool::Vertex(MyVec2f(radius,radius,0));
	MyGraphicsTool::TextureCoordinate(MyVec2f(0,1,0));
	MyGraphicsTool::Vertex(MyVec2f(-radius,radius,0));
	MyGraphicsTool::EndPrimitive();
	mHighlightTextureHelper.DeactiveTexture();
	MyGraphicsTool::DisableTexture2D();
}

void MyVisualElementSingleValueMapper::DrawSaturationHighlight(float value) const{
	float radius = 0.5f;
	radius *= (1+mHighlightBoarder);
	MyGraphicsTool::EnableTexture2D();
	mHighlightTextureHelper.ActiveTexture();
	MyGraphicsTool::EnableAlplaBlending();
	MyGraphicsTool::BeginQuads();
	MyGraphicsTool::TextureCoordinate(MyVec2f(0,0,0));
	MyGraphicsTool::Vertex(MyVec2f(-radius,-radius,0));
	MyGraphicsTool::TextureCoordinate(MyVec2f(1,0,0));
	MyGraphicsTool::Vertex(MyVec2f(radius,-radius,0));
	MyGraphicsTool::TextureCoordinate(MyVec2f(1,1,0));
	MyGraphicsTool::Vertex(MyVec2f(radius,radius,0));
	MyGraphicsTool::TextureCoordinate(MyVec2f(0,1,0));
	MyGraphicsTool::Vertex(MyVec2f(-radius,radius,0));
	MyGraphicsTool::EndPrimitive();
	mHighlightTextureHelper.DeactiveTexture();
	MyGraphicsTool::DisableTexture2D();
}

void MyVisualElementSingleValueMapper::DrawHueHighlight(float value) const{
	float radius = 0.5f;
	radius *= (1+mHighlightBoarder);
	MyGraphicsTool::EnableTexture2D();
	mHighlightTextureHelper.ActiveTexture();
	MyGraphicsTool::EnableAlplaBlending();
	MyGraphicsTool::BeginQuads();
	MyGraphicsTool::TextureCoordinate(MyVec2f(0,0,0));
	MyGraphicsTool::Vertex(MyVec2f(-radius,-radius,0));
	MyGraphicsTool::TextureCoordinate(MyVec2f(1,0,0));
	MyGraphicsTool::Vertex(MyVec2f(radius,-radius,0));
	MyGraphicsTool::TextureCoordinate(MyVec2f(1,1,0));
	MyGraphicsTool::Vertex(MyVec2f(radius,radius,0));
	MyGraphicsTool::TextureCoordinate(MyVec2f(0,1,0));
	MyGraphicsTool::Vertex(MyVec2f(-radius,radius,0));
	MyGraphicsTool::EndPrimitive();
	mHighlightTextureHelper.DeactiveTexture();
	MyGraphicsTool::DisableTexture2D();
}

void MyVisualElementSingleValueMapper::DrawTextureHighlight(float value) const{
	float radius = 0.5;
	radius *= (1+mHighlightBoarder);
	MyGraphicsTool::EnableTexture2D();
	mHighlightTextureHelper.ActiveTexture();
	MyGraphicsTool::EnableAlplaBlending();
	MyGraphicsTool::BeginQuads();
	MyGraphicsTool::TextureCoordinate(MyVec2f(0,0,0));
	MyGraphicsTool::Vertex(MyVec2f(-radius,-radius,0));
	MyGraphicsTool::TextureCoordinate(MyVec2f(1,0,0));
	MyGraphicsTool::Vertex(MyVec2f(radius,-radius,0));
	MyGraphicsTool::TextureCoordinate(MyVec2f(1,1,0));
	MyGraphicsTool::Vertex(MyVec2f(radius,radius,0));
	MyGraphicsTool::TextureCoordinate(MyVec2f(0,1,0));
	MyGraphicsTool::Vertex(MyVec2f(-radius,radius,0));
	MyGraphicsTool::EndPrimitive();
	mHighlightTextureHelper.DeactiveTexture();
	MyGraphicsTool::DisableTexture2D();
}

void MyVisualElementSingleValueMapper::DrawDensityHighlight(float value) const{
	float radius = 0.5;
	radius *= (1+mHighlightBoarder);
	MyGraphicsTool::EnableTexture2D();
	mHighlightTextureHelper.ActiveTexture();
	MyGraphicsTool::EnableAlplaBlending();
	MyGraphicsTool::BeginQuads();
	MyGraphicsTool::TextureCoordinate(MyVec2f(0,0,0));
	MyGraphicsTool::Vertex(MyVec2f(-radius,-radius,0));
	MyGraphicsTool::TextureCoordinate(MyVec2f(1,0,0));
	MyGraphicsTool::Vertex(MyVec2f(radius,-radius,0));
	MyGraphicsTool::TextureCoordinate(MyVec2f(1,1,0));
	MyGraphicsTool::Vertex(MyVec2f(radius,radius,0));
	MyGraphicsTool::TextureCoordinate(MyVec2f(0,1,0));
	MyGraphicsTool::Vertex(MyVec2f(-radius,radius,0));
	MyGraphicsTool::EndPrimitive();
	mHighlightTextureHelper.DeactiveTexture();
	MyGraphicsTool::DisableTexture2D();
}

void MyVisualElementSingleValueMapper::DrawShapeHighlight(float value) const{
	float radius = 0.5;
	radius *= (1+mHighlightBoarder);
	MyGraphicsTool::EnableTexture2D();
	mHighlightTextureHelper.ActiveTexture();
	MyGraphicsTool::EnableAlplaBlending();
	MyGraphicsTool::BeginQuads();
	MyGraphicsTool::TextureCoordinate(MyVec2f(0,0,0));
	MyGraphicsTool::Vertex(MyVec2f(-radius,-radius,0));
	MyGraphicsTool::TextureCoordinate(MyVec2f(1,0,0));
	MyGraphicsTool::Vertex(MyVec2f(radius,-radius,0));
	MyGraphicsTool::TextureCoordinate(MyVec2f(1,1,0));
	MyGraphicsTool::Vertex(MyVec2f(radius,radius,0));
	MyGraphicsTool::TextureCoordinate(MyVec2f(0,1,0));
	MyGraphicsTool::Vertex(MyVec2f(-radius,radius,0));
	MyGraphicsTool::EndPrimitive();
	mHighlightTextureHelper.DeactiveTexture();
	MyGraphicsTool::DisableTexture2D();

}

const MyScale<float, float>& MyVisualElementSingleValueMapper::GetScale() const{
	return mScale;
}

const MyScale<float, MyColor4f>& MyVisualElementSingleValueMapper::GetSaturationScale() const{
	return mSaturationScale;
}

const MyScale<float, MyColor4f>& MyVisualElementSingleValueMapper::GetHueScale() const{
	return mHueScale;
}

int MyVisualElementSingleValueMapper::GetRand(int i) const{
	return mRands[(i*(mRandomSeed+1))%mRands.size()];
}

void MyVisualElementSingleValueMapper::SetRandomSeed(int i){
	mRandomSeed = i;
}

void MyVisualElementSingleValueMapper::SetHighlightMode(bool b){
	mHighlightMode = b;
}

void MyVisualElementSingleValueMapper::SetHighlightColor(const MyColor4f& color){
	mHighlightColor = color;
}

float MyVisualElementSingleValueMapper::GetMaxLength() const{
	return MAXLENGTH;
}

void MyVisualElementSingleValueMapper::DrawHighlightBack(float value) const{
	
	MyPrimitiveDrawer::Color(MyColor4f::black());
	switch (mVisualElement){
		case MyVis::NONE:
			break;
		case MyVis::LENGTH:
			this->DrawLengtHighlight(value);
			break;
		case MyVis::SIZE:
			this->DrawSizeHighlight(value);
			break;
		case MyVis::ANGLE:
			this->DrawAngleHighlight(value);
			break;
		case MyVis::SATURATION:
			this->DrawSaturationHighlight(value);
			break;
		case MyVis::HUE:
			this->DrawHueHighlight(value);
			break;
		case MyVis::TEXTURE:
			this->DrawTextureHighlight(value);
			break;
		case MyVis::DENSITY:
			this->DrawDensityHighlight(value);
			break;
		case MyVis::SHAPE:
			this->DrawShapeHighlight(value);
			break;
		case MyVis::SLOPE:
			this->DrawSlopeHighlight(value);
			break;
		default:
			break;
	}
}

MyBox2f MyVisualElementSingleValueMapper::GetMarkerBox(float value) const{
	MyBox2f box(MyVec2f(-0.5,-0.5),MyVec2f(0.5,0.5));
	switch (mVisualElement){
		case MyVis::NONE:
			return box;
			break;
		case MyVis::LENGTH:
			return MyBox2f(MyVec2f(-0.18,0), MyVec2f(0.18, mScale[value]*MAXLENGTH));
			break;
		case MyVis::SIZE:
			return box;
			break;
		case MyVis::ANGLE:
			return box;
			break;
		case MyVis::SATURATION:
			return box;
			break;
		case MyVis::HUE:
			return box;
			break;
		case MyVis::TEXTURE:
			return box;
			break;
		case MyVis::DENSITY:
			return box;
			break;
		case MyVis::SHAPE:
			return box;
			break;
		case MyVis::SLOPE:
			return box;
			break;
		default:
			return box;
			break;
	}
}