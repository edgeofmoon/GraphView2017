#pragma once
#include "mygraph.h"
#include <map>
#include <string>
#include <vector>

template<typename T>
class MyNodeNamedGraph :
	public MyGraph
{
public:
	MyNodeNamedGraph(void);
	~MyNodeNamedGraph(void);

	const int GetNodeIndex(std::string name) const;
	const std::string GetNodeName(int i) const;

	void SetNodeNames(const std::vector<std::string>& names);
	void SetNodeNames(const std::map<std::string,int>& names);

protected:
	std::map<std::string,int> mNodeNameToIndexMap;
};


template<typename T>
MyNodeNamedGraph::MyNodeNamedGraph(void){

}
	
template<typename T>
MyNodeNamedGraph::~MyNodeNamedGraph(void){
}

template<typename T>
const int MyNodeNamedGraph::GetNodeIndex(std::string name) const{
	return mNodeNameToIndexMap.at[name];
}

template<typename T>
const std::string MyNodeNamedGraph::GetNodeName(int i) const{
	std::map<int, std::string>::const_iterator it;
	for (it = mNodeNameToIndexMap.begin();
		it != mNodeNameToIndexMap.end();
		++it){
		if (it->second == i){
			return it->first;
		}
	}
}

template<typename T>
void MyNodeNamedGraph::SetNodeNames(const std::vector<std::string>& names){
	mNodeNameToIndexMap.clear();
	for(int i = 0;i<names.size();i++){
		mNodeNameToIndexMap[names[i]] = i;
	}
}

template<typename T>
void MyNodeNamedGraph::SetNodeNames(const std::map<std::string,int>& names){
	mNodeNameToIndexMap = names;
}