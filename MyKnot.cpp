#include "MyKnot.h"
#include "MyUtility.h"
#include "MyGenericHierarchyOverlay.h"

int MyKnot::mGlobalMessageIndex = 0;

MyKnot::MyKnot(MyKnot* parent)
	: MyGenericNode(parent)
{
	mMessageIndex = -1;
	mSelectionLevel = 0;
}


MyKnot::~MyKnot(void)
{
	for(int i = 0;i<mSelections.size();i++){
		mSelections[i]->DestoryAll();
		SafeFreeObject(mSelections[i]);
	}
}

void MyKnot::BroadCastMessage(int type, bool responseFirst){
	mGlobalMessageIndex++;
	this->conveyBroadcast(this, mGlobalMessageIndex, type, responseFirst);
}


void MyKnot::SetSelectionLevel(int i){
	mSelectionLevel = i;
}

int MyKnot::GetSelectionLevel() const{
	return mSelectionLevel;
}

void MyKnot::ReplaceSelection(MyGenericNode* selection){
	this->ClearSelections();
	if(selection){
		mSelections.push_back(selection);
	}
}

void MyKnot::AddSelection(MyGenericNode* selection){
	if(selection){
		mSelections.push_back(selection);
	}
}

void MyKnot::RemoveSelection(const MyGenericNode* selection){
	/*
	for(int i = 0;i<mSelections.size();i++){
		if(MyGenericHierarchyOverlayStr::IsObjectEqual(mSelections[i], selection)){
			mSelections[i]->DestoryAll();
			SafeFreeObject(mSelections[i]);
			mSelections.EraseAt(i);
			i--;
		}
		else{
			dynamic_cast<MyGenericHierarchyOverlayStr*>(mSelections[i])
				->CutDescendantObjectNode(selection);
		}
	}
	*/
	for(int i = 0;i<mSelections.size();i++){
		if(MyGenericHierarchyOverlayStr::IsObjectEqual(mSelections[i], selection)){
			for(int is = 0;is<selection->GetNumChildren();is++){
				dynamic_cast<MyGenericHierarchyOverlayStr*>(mSelections[i])->CutDescendantObjectNode(selection->GetChild(is));
			}
		}
		if(!mSelections[i]->HasAnyChild()){
			mSelections.EraseAt(i);
			i--;
		}
	}

}

bool MyKnot::DoIncludeSelection(const MyGenericNode* selection) const{
	if(!selection) return false;
	for(int ic = 0;ic<selection->GetNumChildren();ic++){
		/*
		if(MyGenericHierarchyOverlayStr::IsObjectEqual(mSelections[i], selection)){
			return true;
		}
		if(dynamic_cast<MyGenericHierarchyOverlayStr*>(mSelections[i])
				->HasDescendantObjectNode(selection)){
			return true;
		}
		*/
		bool thisHas = false;
		for(int i = 0;i<mSelections.size();i++){
			if(MyGenericHierarchyOverlayStr::IsObjectEqual(mSelections[i], selection)){
				if(dynamic_cast<MyGenericHierarchyOverlayStr*>(mSelections[i])
					->HasDescendantObjectNode(selection->GetChild(ic))){
					thisHas = true;
					break;
				}
			}
		}
		if(!thisHas) return false;
	}
	return true;
}

const MyGenericNode* MyKnot::GetSelection(int i) const{
	return mSelections[i];
}

void MyKnot::ClearSelections(){
	for(int i = 0;i<mSelections.size();i++){
		mSelections[i]->DestoryAll();
		SafeFreeObject(mSelections[i]);
	}
	mSelections.clear();
}

int MyKnot::GetNumSelections() const{
	return mSelections.size();
}

void MyKnot::AddStaticSelection(MyGenericNode* selection){
	if(selection){
		mStaticSelections.push_back(selection);
	}
}

void MyKnot::conveyBroadcast(const MyKnot* source, int msgIdx, int type, bool responseFirst){
	if(mMessageIndex<msgIdx){
		mMessageIndex = msgIdx;;
		if(responseFirst){
			this->responseBroadcast(source,type);
		}
		for(int i = 0;i<this->GetNumParents();i++){
			MyKnot* parent = dynamic_cast<MyKnot*>(this->GetParent(i));
			parent->conveyBroadcast(this, msgIdx, type, responseFirst);
		}
		for(int i = 0;i<this->GetNumChildren();i++){
			MyKnot* child = dynamic_cast<MyKnot*>(this->GetChild(i));
			child->conveyBroadcast(this, msgIdx, type, responseFirst);
		}
		if(!responseFirst){
			this->responseBroadcast(source,type);
		}
	}
}

void MyKnot::responseBroadcast(const MyKnot* source, int type){
	// do nothing
}

void MyKnot::replaceSelections(const MyKnot* source){
	this->ClearSelections();
	for(int i = 0;i<source->GetNumSelections();i++){
		this->AddSelection(source->GetSelection(i)->MakeCopy());
	}

}