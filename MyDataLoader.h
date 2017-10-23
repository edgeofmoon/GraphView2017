#pragma once

#include "MyMatrix.h"
#include "MyVec.h"
#include "MyGenericNode.h"
#include "MyGenericGraph.h"
#include "MyArray.h"
#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"
#include "MyDenseGraph.h"
#include "MyString.h"

#include <string>
#include <map>

struct nodeInfo_t{
	std::string id;
	std::string color;
	std::string label;
	std::string name;

};
typedef struct nodeInfo_t NodeInfo;

struct edgeInfo_t{
	// int is the string value minus 1
	int source;
	int target;

	float weight;
};
typedef struct edgeInfo_t EdgeInfo;

class MyDataLoader
{
public:
	MyDataLoader(void);
	~MyDataLoader(void);

	static MyArray3f* MakeArrayVecFromFile(std::string fileName, int n = -1);
	static MyMatrixf* MakeMatrixFromFile(std::string fileName, int m = -1, int n = -1);
	static MyArrayStr* MakeArrayStrFromFile(std::string fileName);
	static MyDenseGraphf* MakeGraphFromMatrixFileFilterByEdgeWeight(const MyString& matFile, int m, int n, float thres);
	static MyDenseGraphf* MakeGraphFromMatrixFileFilterByEdgeRatio(const MyString& matFile, int m, int n, float ratio);

	// only works for 3 levels for now
	static MyGenericNode* LoadHierarchyOverlay(std::string fileName);

	static void MakeGraphFromXmlFile(std::string fileName, MyMatrixf*& mat,
		MyArray3f*& pos, MyArrayStr*& label);

	static void PrintCantOpenFileMsg(std::string fileName);

private:

	static std::map<std::string, std::string> nameToIdName;
	static std::vector<NodeInfo> nodeInfos;
	static std::vector<EdgeInfo> edgeInfos;
	static std::vector<NodeInfo>* nodeInfosPtr;
	static std::vector<EdgeInfo>* edgeInfosPtr;
	static void buildAttriNameMap(const rapidxml::xml_node<> *graphmlNode);
	static void buildNodeLabels(const rapidxml::xml_node<> *graphmlNode);
	static void buildEdgeInfos(const rapidxml::xml_node<> *graphmlNode);
};

