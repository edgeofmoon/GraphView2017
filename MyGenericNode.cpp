#include "MyGenericNode.h"
#include <algorithm>
using namespace std;

int MyGenericNode::mNumNodeInTotal = 0;
int MyGenericNode::mDepth = 0;
MyArray<const MyGenericNode*> MyGenericNode::mArray;

MyGenericNode::MyGenericNode(MyGenericNode* parent)
{
	Connect(parent,this);
	mNumNodeInTotal++;
}


MyGenericNode::~MyGenericNode(void)
{
	mNumNodeInTotal--;
}

void MyGenericNode::CopyAll(const MyGenericNode* node){
	this->CopyAscendants(node);
	this->CopyDescendants(node);
	this->CopyAuxiliaryContent(node);
}

void MyGenericNode::CopyAscendants(const MyGenericNode* node){
	mParents.resize(node->GetNumParents());
	for(int i = 0;i<node->GetNumParents();i++){
		const MyGenericNode* parent = node->GetParent(i);
		mParents[i] = parent->MakeNew();
		mParents[i]->addChild(this);
		mParents[i]->CopyAscendants(parent);
		mParents[i]->CopyAuxiliaryContent(parent);
	}
}

void MyGenericNode::CopyDescendants(const MyGenericNode* node){
	mChildren.resize(node->GetNumChildren());
	for(int i = 0;i<node->GetNumChildren();i++){
		const MyGenericNode* child = node->GetChild(i);
		mChildren[i] = child->MakeNew();
		mChildren[i]->addParent(this);
		mChildren[i]->CopyDescendants(child);
		mChildren[i]->CopyAuxiliaryContent(child);
	}
}

MyGenericNode* MyGenericNode::MakeCopy() const{
	MyGenericNode* copy = this->MakeNew();
	copy->CopyAll(this);
	return copy;
}
void MyGenericNode::CopyAuxiliaryContent(const MyGenericNode* node){
}

MyGenericNode* MyGenericNode::MakeNew() const{
	return new MyGenericNode;
}

int MyGenericNode::GetNumLeaves() const{
	if(!this->HasAnyChild()){
		return 1;
	}
	int leaves = 0;
	for(int i = 0;i< this->GetNumChildren();i++){
		leaves += this->GetChild(i)->GetNumLeaves();
	}
	return leaves;
}

int MyGenericNode::Connect(MyGenericNode* parent,
	MyGenericNode* child){
	if(0 == parent || 0 == child){
		return -1;
	}
	else{
		int addChildGood = parent->addChild(child);
		int addParentGood = child->addParent(parent);
		return (addParentGood << 1) + addChildGood;
	}
}

int MyGenericNode::Disconnect(MyGenericNode* parent,
	MyGenericNode* child){
	if(0 == parent || 0 == child){
		return -1;
	}
	else{
		int removeChildGood = parent->removeChild(child);
		int removeParentGood = child->removeParent(parent);
		return (removeParentGood << 1) + removeChildGood;
	}
}

int MyGenericNode::addChild(MyGenericNode* child){
	if(0 == child) return -1;
	if(find(mChildren.begin(),mChildren.end(),child)==mChildren.end()){
		mChildren.push_back(child);
		return 1;
	}
	return 0;
}

bool MyGenericNode::HasDescendantNode(const MyGenericNode* node) const{
	for(int i = 0;i<mChildren.size();i++){
		if(mChildren[i] == node){
			return true;
		}
	}
	for(int i = 0;i<mChildren.size();i++){
		if(mChildren[i]->HasDescendantNode(node)){
			return true;
		}
	}
	return false;
}

bool MyGenericNode::CutDescendantNode(MyGenericNode* node){
	if(this == node){
		this->DestoryDescendants();
		return true;
	}
	for (int i = 0; i < mChildren.size(); i++){
		if(mChildren[i] == node){
			mChildren[i] ->CutDescendantNode(node);
			delete mChildren[i];
			mChildren.EraseAt(i);
			return true;
		}
	}
	return false;
}

void MyGenericNode::DestoryAll(){
	this->DestoryAscendants();
	this->DestoryDescendants();
}

void MyGenericNode::DestoryAscendants(){
	if(!HasAnyParent()) return;
	else{
		for (int i = 0; i < mParents.size(); i++){
			mParents[i]->DestoryAscendants();
			delete mParents[i];
		}
		mParents.clear();
	}
}

void MyGenericNode::DestoryDescendants(){
	if(!HasAnyChild()) return;
	else{
		for (int i = 0; i < mChildren.size(); i++){
			mChildren[i]->DestoryDescendants();
			delete mChildren[i];
		}
		mChildren.clear();
	}
}

void MyGenericNode::TraverseDescendants(void(*func)(MyGenericNode*)){
	func(this);
	if(!this->HasAnyChild()){
	}
	else{
		for(int i = 0;i<this->GetNumChildren();i++){
			this->GetChild(i)->TraverseDescendants(func);
		}
	}
}

void MyGenericNode::TraverseDescendants(void(*func)(const MyGenericNode*)) const{
	func(this);
	if(!this->HasAnyChild()){
	}
	else{
		for(int i = 0;i<this->GetNumChildren();i++){
			this->GetChild(i)->TraverseDescendants(func);
		}
	}
}

int MyGenericNode::GetHeight() const{
	if(!this->HasAnyChild()){
		return 0;
	}
	else{
		int maxChildHeight = 0;
		for(int i = 0;i<this->GetNumChildren();i++){
			int childHeight = this->GetChild(i)->GetHeight();
			if(childHeight>maxChildHeight){
				maxChildHeight = childHeight;
			}
		}
		return maxChildHeight + 1;
	}
}

int MyGenericNode::GetDepth() const{
	if(!this->HasAnyParent()){
		return 0;
	}
	else{
		int maxParentDepth = 0;
		for(int i = 0;i<this->GetNumParents();i++){
			int parentDepth = this->GetParent(i)->GetDepth();
			if(parentDepth>maxParentDepth){
				maxParentDepth = parentDepth;
			}
		}
		return maxParentDepth + 1;
	}
}


int MyGenericNode::addParent(MyGenericNode* parent){
	if(0 == parent) return -1;
	if(find(mParents.begin(),mParents.end(),parent)==mParents.end()){
		mParents.push_back(parent);
		return 1;
	}
	return 0;
}

int MyGenericNode::removeChild(MyGenericNode* child){
	if(0 == child) return -1;
	vector<MyGenericNode*>::iterator itr 
		= find(mChildren.begin(),mChildren.end(),child);
	if(itr!=mChildren.end()){
		mChildren.erase(itr);
		return 1;
	}
	return 0;
}
int MyGenericNode::removeParent(MyGenericNode* parent){
	if(0 == parent) return -1;
	vector<MyGenericNode*>::iterator itr 
		= find(mParents.begin(),mParents.end(),parent);
	if(itr!=mParents.end()){
		mParents.erase(itr);
		return 1;
	}
	return 0;
}

MyArray<const MyGenericNode*>* MyGenericNode::MakeArrayWithCutLeaveNoDeeperThan(int depth) const{
	MyArray<const MyGenericNode*>* rst = new MyArray<const MyGenericNode*>;
	MyGenericNode::mDepth = depth;
	this->TraverseDescendants(MyGenericNode::addDepth);
	*rst = MyGenericNode::mArray;
	MyGenericNode::mArray.clear();
	return rst;
}

void MyGenericNode::addDepth(const MyGenericNode* node){
	int nodeDepth = node->GetDepth();
	if(nodeDepth == MyGenericNode::mDepth
		|| (!node->HasAnyChild() && nodeDepth< MyGenericNode::mDepth)){
		MyGenericNode::mArray.push_back(node);
	}
}