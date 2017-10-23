#pragma once

#include "MyArray.h"
#include "MyString.h"

#include "MyGenericNode.h"

template<typename T>
class MyGenericHierarchyOverlay
	: public MyGenericNode
{
public:
	MyGenericHierarchyOverlay();
	MyGenericHierarchyOverlay(const T& obj);
	virtual ~MyGenericHierarchyOverlay();

	virtual MyGenericNode* MakeNew() const;
	virtual void CopyAuxiliaryContent(const MyGenericNode* node);

	T GetObject() const;
	void SetObject(const T& obj);
	
	int AddChild(MyGenericNode* child);
	int RemoveChild(MyGenericNode* child);

	MyArray<T> GetChildrenObjects() const;
	
	virtual bool HasDescendantObjectNode(const MyGenericNode* node) const;
	virtual bool CutDescendantObjectNode(const MyGenericNode* node);

	static const MyGenericNode* GetOneWithObject(const T& obj, const MyGenericNode* root);
	static void AddLeaveObjectToArray( MyArray<T>* arr, const MyGenericNode* root);
	
	static bool IsObjectEqual(const MyGenericNode* node1, const MyGenericNode* node2);
	static int GetIndexOfObject(const MyArray<const MyGenericNode*>* arr, const T& obj);
protected:
	// this is used to indicate the actual
	// underlying object
	T mObject;
};

template<typename T>
MyGenericHierarchyOverlay<T>::MyGenericHierarchyOverlay(){
}

template<typename T>
MyGenericHierarchyOverlay<T>::MyGenericHierarchyOverlay(const T& obj){
	mObject = obj;
}

template<typename T>
MyGenericHierarchyOverlay<T>::~MyGenericHierarchyOverlay(){
	//DestoryAll();
}

template<typename T>
MyGenericNode* MyGenericHierarchyOverlay<T>::MakeNew() const{
	return new MyGenericHierarchyOverlay<T>;
}

template<typename T>
void MyGenericHierarchyOverlay<T>::CopyAuxiliaryContent(const MyGenericNode* node){
	mObject = dynamic_cast<const MyGenericHierarchyOverlay<T>*>(node)->GetObject();
}

template<typename T>
T MyGenericHierarchyOverlay<T>::GetObject() const{
	return mObject;
}

template<typename T>
void MyGenericHierarchyOverlay<T>::SetObject(const T& obj){
	mObject = obj;
}

template<typename T>
int MyGenericHierarchyOverlay<T>::AddChild(MyGenericNode* child){
	return MyGenericNode::Connect(this, child);
}

template<typename T>
int MyGenericHierarchyOverlay<T>::RemoveChild(MyGenericNode* child){
	return MyGenericNode::Disconnect(this, child);
}

template<typename T>
MyArray<T> MyGenericHierarchyOverlay<T>::GetChildrenObjects() const{
	MyArray<T> childrenObjects;
	int numChildren = this->GetNumChildren();
	childrenObjects.reserve(numChildren);
	for (int i = 0; i < numChildren; i++){
		const MyGenericHierarchyOverlay<T>* child = 
			dynamic_cast<const MyGenericHierarchyOverlay<T>*>(this->GetChild(i));
		if(child){
			childrenObjects.push_back(child->GetObject());
		}
	}
	return childrenObjects;
}

template<typename T>
const MyGenericNode* MyGenericHierarchyOverlay<T>::GetOneWithObject(const T& obj, const MyGenericNode* root){
	const MyGenericNode* rst = 0;
	if(!root->HasAnyChild()){
		const MyGenericHierarchyOverlay<T>* inst
			= dynamic_cast<const MyGenericHierarchyOverlay<T>*>(root);
		if(inst){
			if(inst->GetObject() == obj){
				rst = root;
			}
		}
	}
	else{
		for(int i = 0;i<root->GetNumChildren();i++){
			rst = GetOneWithObject(obj, root->GetChild(i));
			if(rst){
				break;
			}
		}
	}
	return rst;
}

template<typename T>
void MyGenericHierarchyOverlay<T>::AddLeaveObjectToArray( MyArray<T>* arr, const MyGenericNode* root){
	if(!root) return;
	if (!root->HasAnyChild()){
		arr->push_back(dynamic_cast<const MyGenericHierarchyOverlay<T>*>(root)->GetObject());
	}
	else{
		for(int i = 0;i<root->GetNumChildren();i++){
			AddLeaveObjectToArray(arr, root->GetChild(i));
		}
	}
}

template<typename T>
bool MyGenericHierarchyOverlay<T>::IsObjectEqual(const MyGenericNode* node1, const MyGenericNode* node2){
	const MyGenericHierarchyOverlay<T>* node1_cast
		= dynamic_cast<const MyGenericHierarchyOverlay<T>*>(node1);
	if(node1_cast){
		const MyGenericHierarchyOverlay<T>* node2_cast
			= dynamic_cast<const MyGenericHierarchyOverlay<T>*>(node2);
		if(node2_cast){
			if(node1_cast->GetObject() == node2_cast->GetObject()){
				return true;
			}
		}
	}
	return false;
}

template<typename T>
int MyGenericHierarchyOverlay<T>::GetIndexOfObject(const MyArray<const MyGenericNode*>* arr, const T& obj){
	for(int i = 0;i<arr->size();i++){
		const MyGenericHierarchyOverlay<T>* overlay
			= dynamic_cast<const MyGenericHierarchyOverlay<T>*>(arr->at(i));
		if(overlay){
			if(overlay->GetObject() == obj){
				return i;
			}
		}
	}
	return -1;
}

template<typename T>
bool MyGenericHierarchyOverlay<T>::HasDescendantObjectNode(const MyGenericNode* node) const{
	for(int i = 0;i<mChildren.size();i++){
		if(MyGenericHierarchyOverlay<T>::IsObjectEqual(mChildren[i], node)){
			return true;
		}
	}
	for(int i = 0;i<mChildren.size();i++){
		if(dynamic_cast<const MyGenericHierarchyOverlay<T>*>(mChildren[i])
			->HasDescendantObjectNode(node)){
			return true;
		}
	}
	return false;
}

template<typename T>
bool MyGenericHierarchyOverlay<T>::CutDescendantObjectNode(const MyGenericNode* node){
	if(MyGenericHierarchyOverlay<T>::IsObjectEqual(this, node)){
		this->DestoryDescendants();
		return true;
	}
	for (int i = 0; i < mChildren.size(); i++){
		if(MyGenericHierarchyOverlay<T>::IsObjectEqual(mChildren[i], node)){
			dynamic_cast<MyGenericHierarchyOverlay<T>*>(mChildren[i])
				->CutDescendantObjectNode(node);
			delete mChildren[i];
			mChildren.EraseAt(i);
			return true;
		}
	}
	return false;
}

typedef MyGenericHierarchyOverlay<int> MyGenericHierarchyOverlayi;
typedef MyGenericHierarchyOverlay<MyVec2i> MyGenericHierarchyOverlay2i;
typedef MyGenericHierarchyOverlay<MyString> MyGenericHierarchyOverlayStr;