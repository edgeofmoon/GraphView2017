#include "MyDataLoader.h"
#include "MyGenericHierarchyOverlay.h"
#include "MyString.h"
#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"

#include "MyGraphFilter.h"

#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <map>
#include <algorithm>
using namespace std;
using namespace rapidxml;

MyDataLoader::MyDataLoader(void)
{
}


MyDataLoader::~MyDataLoader(void)
{
}

MyArray3f* MyDataLoader::MakeArrayVecFromFile(std::string fileName, int n){
	MyArray3f* vecs = 0;
	ifstream inFile(fileName);
	if(inFile.is_open()){
		vecs = new MyArray3f;
		for(int i = 0;i<n;i++){
			MyVec3f vec;
			inFile >> vec[0] >> vec[1] >> vec[2];
			vecs->push_back(vec);
		}
		inFile.close();
	}
	else{
		PrintCantOpenFileMsg(fileName);
	}
	return vecs;
}

MyMatrixf* MyDataLoader::MakeMatrixFromFile(std::string fileName, int m, int n){
	MyMatrixf* mat = 0;
	ifstream inFile(fileName);
	if(inFile.is_open()){
		mat = new MyMatrixf(m,n);
		for(int i = 0;i<m;i++){
			for(int j = 0;j<n;j++){
				inFile >> mat->At(i,j);
			}
		}
		inFile.close();
	}
	else{
		PrintCantOpenFileMsg(fileName);
	}
	return mat;
}

MyArrayStr* MyDataLoader::MakeArrayStrFromFile( std::string fileName){
	MyArrayStr* vecs = 0;
	ifstream inFile(fileName);
	if(inFile.is_open()){
		vecs = new MyArrayStr;
		while(!inFile.eof()){
			char tmp[1000];
			inFile.getline(tmp,1000);
			std::stringstream ss(tmp);
			MyString name;
			ss >> name;
			vecs->push_back(name);
		}
		inFile.close();
	}
	else{
		PrintCantOpenFileMsg(fileName);
	}
	return vecs;
}

MyDenseGraphf* MyDataLoader::MakeGraphFromMatrixFileFilterByEdgeWeight(const MyString& matFile, int m, int n, float thres){
	//ifstream infile(matFile);
	//if(!infile.is_open()){
	//	PrintCantOpenFileMsg(matFile);
	//	return 0;
	//}
	MyMatrixf* mat = MyDataLoader::MakeMatrixFromFile(matFile, m, n);
	MyDenseGraphf* graph = new MyDenseGraphf;
	graph->SetMatrix(mat);
	MyGraphFilter* graphFilter = new MyGraphFilter;
	graphFilter->SetInputGraph(graph);
	MyDenseGraphf* newGraph = dynamic_cast<MyDenseGraphf*>(graphFilter->MakeFilteringByEdgeWeight(thres, 999999.1f));
	delete graph;
	return newGraph;
}

MyDenseGraphf* MyDataLoader::MakeGraphFromMatrixFileFilterByEdgeRatio(const MyString& matFile, int m, int n, float ratio){
	//ifstream infile(matFile);
	//if(!infile.is_open()){
	//	PrintCantOpenFileMsg(matFile);
	//	return 0;
	//}
	MyMatrixf* mat = MyDataLoader::MakeMatrixFromFile(matFile, m, n);
	MyDenseGraphf* graph = new MyDenseGraphf;
	graph->SetMatrix(mat);
	MyGraphFilter* graphFilter = new MyGraphFilter;
	graphFilter->SetInputGraph(graph);
	MyDenseGraphf* newGraph = dynamic_cast<MyDenseGraphf*>(graphFilter->MakeFilteringByEdgeWeightRatio(ratio));
	delete graph;
	return newGraph;
}

MyGenericNode* MyDataLoader::LoadHierarchyOverlay(std::string fileName){
	MyGenericHierarchyOverlayStr* root = 0;
	ifstream inFile(fileName);
	if(inFile.is_open()){
		char tmp[1000];
		inFile.getline(tmp,1000);
		root = new MyGenericHierarchyOverlayStr(std::string(tmp));
		while(!inFile.eof()){
			inFile.getline(tmp,1000);
			MyGenericHierarchyOverlayStr* cluster = new MyGenericHierarchyOverlayStr(std::string(tmp));
			std::string line;
			std::getline( inFile, line ) ;
			std::istringstream is( line );
			std::vector<int> indices = std::vector<int>(std::istream_iterator<int>(is),std::istream_iterator<int>());
			if(indices.empty()){
				delete cluster;
			}
			else{
				for(int i = 0;i<indices.size();i++){
					MyGenericHierarchyOverlayStr* child = new MyGenericHierarchyOverlayStr(MyString(indices[i]));
					cluster->AddChild(child);
				}
				root->AddChild(cluster);
			}
		}
		inFile.close();
	} 
	else{
		PrintCantOpenFileMsg(fileName);
	}
	return root;
}



map<string, string> MyDataLoader::nameToIdName;
vector<NodeInfo> MyDataLoader::nodeInfos;
vector<EdgeInfo> MyDataLoader::edgeInfos;
vector<NodeInfo>* MyDataLoader::nodeInfosPtr = 0;
vector<EdgeInfo>* MyDataLoader::edgeInfosPtr = 0;

void stringTo3f(string str, float &x, float &y, float& z){
	stringstream ss(str);
	char c;
	ss >> x >> c >> y >> c >> z;
}
bool IsLessId(const NodeInfo& a, const NodeInfo& b){
	return atoi(a.id.c_str()) < atoi(b.id.c_str());
}

void MyDataLoader::buildAttriNameMap(const xml_node<> *graphmlNode){
	nameToIdName.clear();
	for (xml_node<> *node = graphmlNode->first_node("key");
		node;
		node = node->next_sibling("key")){
		string valueName;
		string valueId;
		for (xml_attribute<> *attr = node->first_attribute();
			attr;
			attr = attr->next_attribute()){

			if (string(attr->name()) == string("attr.name")){
				valueName = attr->value();
			}
			else if (string(attr->name()) == string("id")){
				valueId = attr->value();
			}
		}
		nameToIdName[valueName] = valueId;
	}
}

void MyDataLoader::buildNodeLabels(const xml_node<> *graphmlNode){
	nodeInfos.clear();
	for (xml_node<> *node = graphmlNode->first_node("graph")->first_node("node");
		node;
		node = node->next_sibling("node")){
		nodeInfos.push_back(NodeInfo());
		nodeInfos.back().id = node->first_attribute("id")->value();
		for (xml_node<> *dataNode = node->first_node("data");
			dataNode;
			dataNode = dataNode->next_sibling("data")){
			for (xml_attribute<> *attr = dataNode->first_attribute();
				attr;
				attr = attr->next_attribute()){
				if (string(attr->name()) == string("key")){
					if (string(attr->value()) == nameToIdName["dn_label"]){
						nodeInfos.back().label = dataNode->value();
					}
					else if (string(attr->value()) == nameToIdName["dn_freesurfer_structname"]){
						nodeInfos.back().name = dataNode->value();
					}
					else if (string(attr->value()) == nameToIdName["dn_colorcode"]){
						nodeInfos.back().color = dataNode->value();
						float x, y, z;
						stringTo3f(nodeInfos.back().color, x, y, z);
					}
				}
			}
		}
	}
	sort(nodeInfos.begin(), nodeInfos.end(), IsLessId);
	nodeInfosPtr = new vector<NodeInfo>(nodeInfos);
}

void MyDataLoader::buildEdgeInfos(const xml_node<> *graphmlNode){
	edgeInfos.clear();
	for (xml_node<> *edge = graphmlNode->first_node("graph")->first_node("edge");
		edge;
		edge = edge->next_sibling("edge")){
		edgeInfos.push_back(EdgeInfo());
		edgeInfos.back().source = atoi(edge->first_attribute("source")->value()) - 1;
		edgeInfos.back().target = atoi(edge->first_attribute("target")->value()) - 1;
		for (xml_node<> *dataNode = edge->first_node("data");
			dataNode;
			dataNode = dataNode->next_sibling("data")){
			for (xml_attribute<> *attr = dataNode->first_attribute();
				attr;
				attr = attr->next_attribute()){
				if (string(attr->name()) == string("key")){
					if (string(attr->value()) == nameToIdName["de_length"]){
						edgeInfos.back().weight = atof(dataNode->value());
					}
				}
			}
		}
	}
	edgeInfosPtr = new vector<EdgeInfo>(edgeInfos);
}

void MyDataLoader::MakeGraphFromXmlFile(std::string fileName, MyMatrixf*& mat,
	MyArray3f*& pos, MyArrayStr*& label){
	vector<NodeInfo> nodeInfos;
	vector<EdgeInfo> edgeInfos;
	xml_document<> doc;    // character type defaults to char
	file<> xmlFile(fileName.c_str());
	char* text = xmlFile.data();
	doc.parse<0>(text);    // 0 means default parse flags
	//xml_node<> *node1 = doc.first_node("graphml")->first_node("graph")->first_node("node");
	//putHierarchyInfo(node1);
	//xml_node<> *node2 = doc.first_node("graphml")->first_node("graph")->last_node("edge");
	//putHierarchyInfo(node2);
	buildAttriNameMap(doc.first_node("graphml"));

	buildNodeLabels(doc.first_node("graphml"));
	buildEdgeInfos(doc.first_node("graphml"));

	nodeInfos = *nodeInfosPtr;
	edgeInfos = *edgeInfosPtr;
	label = new MyArrayStr;
	pos = new MyArray3f;
	// convert data
	for (int i = 0; i < nodeInfos.size(); i++){
		label->push_back(nodeInfos[i].name);
		MyVec3f p;
		stringTo3f(nodeInfos[i].color, p[0], p[1], p[2]);
		pos->push_back(p);
	}

	mat = new MyMatrixf(nodeInfos.size(), nodeInfos.size());
	for (int i = 0; i < edgeInfos.size(); i++){
		EdgeInfo edgeInfo = edgeInfos[i];
		mat->At(edgeInfo.source, edgeInfo.target) = edgeInfo.weight;
		mat->At(edgeInfo.target, edgeInfo.source) = edgeInfo.weight;
	}
}

void MyDataLoader::PrintCantOpenFileMsg(std::string fileName){
	cout << "Can't Open File "<< fileName << endl;
}