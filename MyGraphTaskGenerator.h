#pragma once

#include "MyArray.h"
#include "MyString.h"
#include "MyGenericGraph.h"
#include "MyVisualElement.h"

#include <ostream>

class VisSpec{
public:
	enum Density{
		SPARSE = 0,
		MEDIUM = 1,
		DENSE = 2
	};
	enum Geometry{
		GEO_RAW = 0,
		GEO_TREERING = 1,
		GEO_MATRIX = 2
	};
	enum Topology{
		TOPO_LINE = 0,
		TOPO_BUNDLED = 1,
		TOPO_MATRIX = 2
	};
	enum Background{
		WHITE = 0,
		GRAY = 1,
	};
	
	VisSpec(){
	}

	VisSpec(Density d, Geometry g, Topology t, MyVis::MyVisualElement ele, Background bg){
		mDensity = d;
		mGeometry = g;
		mTopology = t;
		mVisualElement = ele;
		mBackground = bg;
	}

	float GetDensityValue() const{
		switch (mDensity){
		case VisSpec::SPARSE:
			return 0.05f;
			break;
		case VisSpec::MEDIUM:
			return 0.10f;
			break;
		case VisSpec::DENSE:
			return 0.15f;
			break;
		default:
			return 0.05f;
			break;
		}
	}

	MyString GetGeometryValue() const{
		switch (mGeometry)
		{
		case VisSpec::GEO_TREERING:
			return "layout_treering";
			break;
		case VisSpec::GEO_MATRIX:
			return "layout_matrix";
			break;
		case VisSpec::GEO_RAW:
		default:
			return "layout_default";
			break;
		}
	}
	
	MyString GetTopologyValue() const{
		switch (mTopology)
		{
		case VisSpec::TOPO_LINE:
			return "bundler_none";
			break;
		case VisSpec::TOPO_BUNDLED:
			if(mGeometry == VisSpec::GEO_TREERING){
				return "bundler_HEB";

			}
			else{
				return "bundler_GEB";
			}
			break;
		case VisSpec::TOPO_MATRIX:
		default:
			return "bundler_none";
			break;
		}
	}

	void SetDensity(Density d){ mDensity = d; };
	void SetGeometry(Geometry g){ mGeometry = g; };
	void SetTopoloty(Topology t){ mTopology = t;};

	Density GetDensity() const {return mDensity;};
	Geometry GetGeometry() const {return mGeometry;};
	Topology GetTopology() const {return mTopology;};
	MyVis::MyVisualElement GetVisualElement() const { return mVisualElement; };
	Background GetBackground() const { return mBackground; };

protected:
	Density mDensity;
	Geometry mGeometry;
	Topology mTopology;
	MyVis::MyVisualElement mVisualElement;
	Background mBackground;
};

class MyGraphTaskGenerator
{
public:
	MyGraphTaskGenerator(void);
	~MyGraphTaskGenerator(void);

	void GenerateConfigureFiles();

	void SetMeanMatrixFile(const MyString& file);
	void SetMatrixFiles(const MyArrayStr& files);
	void SetHierarchyFile(const MyString& file);
	void SetPositionFile(const MyString& file);
	void SetLabelFile(const MyString& file);

	void ReadSegmentationFile(const MyString& file);

	const MyArrayStr& GetConfigureFiles() const;

	void WriteConfigure(std::ostream& out, const MyString& task, const VisSpec& spec, MyArrayStr matFiles, const MyArrayi& indices, int regionIndex = -1) const;
	
	MyString GetRepresentation(const VisSpec& spec) const;

	MyArrayi ComputeExtraInfo(const MyString& task, const VisSpec& spec, const MyString& matfile, const MyString& matfile2, int regionIndex = -1) const;

	int GetNodeOfAboutDegree(int degree, const MyGenericGraphf* graph, int regionIndex = -1) const;

	int GetNodePairAboutDegreeDifference(float difference, const MyGenericGraphf* graph1, const MyGenericGraphf* graph2, int regionIndex = -1) const;

	MyVec2i GetNodePairAboutNeighbor(int neighbor, const MyGenericGraphf* graph, const MyGenericGraphf* graph2, int regionIndex = -1) const;
	
	MyVec2i GetNodePairAboutNeighbor(int neighbor, const MyGenericGraphf* graph, int regionIndex = -1) const;

	// these are for new setups
	int GetNodeOfDegreeCentrality(MyVec2f dcRange, const MyGenericGraphf* graph, int regionIndex = -1);

	int GetNodeOfDegreeCentralityDifference(MyVec2f dcDiffRange, const MyGenericGraphf* graph1, const MyGenericGraphf* graph2, int regionIndex = -1);

	MyVec2i GetNodePairOfCommonNeighbor(MyVec2i neighborRange, const MyGenericGraphf* graph, int regionIndex = -1);

	int GetNodeOfHubNeighbor(float hubThres, MyVec2i neighborRange, const MyGenericGraphf* graph, int regionIndex = -1);

	int GetOppositeNodeOfHubNeighbor(float hubThres, MyVec2i neighborRange, const MyGenericGraphf* graph, int regionIndex = -1);

	int GetLobeThatHasHub(float hubThres, const MyGenericGraphf* graph, const MyGenericNode* hierarchy);

	MyVec2i GetInterLobeLinkDifference(MyVec2i intLobDiffRange,const MyGenericGraphf* graph1, const MyGenericGraphf* graph2, const MyGenericNode* hierarchy);

	MyVec2i GetAreasWithVariationDifference(MyVec2i linkChangeDiffRange, const MyGenericGraphf* graph0, const MyGenericGraphf* graph1, const MyGenericNode* hierarchy);
	
	MyArrayi ComputeExtraInfoAtDifficulty(const MyString& task, const VisSpec& spec, const MyString& matfile, const MyString& matfile2, int difficulty = 0, int regIdx = -1);

protected:

	MyArrayStr mMatrixFileNames;
	MyString mMeanMatrixFileNames;
	MyString mHierarchyFileName;
	MyString mPositionFile;
	MyString mLabelFile;

	MyString mDecimer;

	MyString mFolder;
	MyArrayStr mConfigureFiles;

	MyArray<MyArrayi> mSegmentation;

	bool mHealthy;
	MyArray2f mMeanGraphDegreeSegs;
	MyArray2f mDegreeDiffSegs;
	MyArray2i mCommonNeighborSegs;
	MyArrayf mMeanGraphHubNodeThres;
	MyArray2i mNeighborRange;
	MyArray2i mLinkChangeRange;
	MyArray2i mLinkChangeDiffRange;
	MyArray2i mOppositeNeighborRange;
	MyArray2i mInterLobeLinkDiffRange;

	//0~left
	//1~right
	int getSide(MyString name) const;
};

