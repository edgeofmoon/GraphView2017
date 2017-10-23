#include "MyScene.h"
#include "MyView.h"
#include "MyGraphicsTool.h"
#include "MyUtility.h"
#include "MySelectionEvent.h"

#include <iostream>
#include <vector>

MyScene::MyScene(void)
{
	mFocusKnot = 0;
	mLayoutManager.SetKnotContainer(&mKnotContainer);
	mInteractionMode = 0;
}


MyScene::~MyScene(void)
{
}

void MyScene::SetView(MyView* view){
	mView = view;
}

void MyScene::Show(){
	
	for(std::vector<MyKnot*>::iterator
		itrKnot = mKnotContainer.Begin();
		itrKnot != mKnotContainer.End();
		itrKnot ++){
		(*itrKnot)->Show();
	}
}

int MyScene::AddKnot(MyKnot* pKnot){
	return mKnotContainer.AddKnot(pKnot);
}

int MyScene::RemoveKnot(MyKnot* pKnot){
	return mKnotContainer.RemoveKnot(pKnot);
}

int MyScene::EventHandler(MyGenericEvent& eve){
	mView->SetScene(this);
	// click event
	if(this->GetNumKnots()<=0) return 0;
	if(eve.GetEventSource() == MyGenericEvent::MOUSE_KEY
		&& eve.GetEventKeyState() == MyGenericEvent::KEY_DOWN
		&& eve.GetEventMouseKey() == MyGenericEvent::MOUSE_KEY_LEFT
		// accept event only can select the lowest level unit
		&& this->GetKnot(0)->GetSelectionLevel() == 0){
		MyKnot* knot = mView->GetKnotAt(eve.GetMouseX(), eve.GetMouseY());
		
		if(mInteractionMode == 1){
			mFocusKnot = knot;
			this->hintFocusKnot();
		}
		if(knot){
			MyGenericNode* sel = mView->MakeSeeWhat(eve.GetMouseX(), eve.GetMouseY(), knot);
			bool isdone = false;
			/*
			if (sel){
				if (sel->GetNumChildren() > 0){
					for (int i = 0; i < sel->GetNumChildren(); i++){
						MyGenericNode* node = sel->GetChild(i)->MakeCopy();
						if (knot->DoIncludeSelection(node)){
							knot->RemoveSelection(node);
							SafeFreeObject(node);
						}
						else{
							knot->AddSelection(node);
						}
					}
					sel->DestoryAll();
					SafeFreeObject(sel);
					isdone = true;
				}
			}
			*/
			if (!isdone){
				if(knot->DoIncludeSelection(sel)){
					knot->RemoveSelection(sel);
					MySelectionEvent& seleve = dynamic_cast<MySelectionEvent&>(eve);
					seleve.DeriveActionFromSelection(MySelectionEvent::SEL_DELETE, sel, this);
					SafeFreeObject(sel);
				}
				else{
					knot->AddSelection(sel);
					MySelectionEvent& seleve = dynamic_cast<MySelectionEvent&>(eve);
					seleve.DeriveActionFromSelection(MySelectionEvent::SEL_ADD, sel, this);
				}
			}
			knot->BroadCastMessage(MyKnot::BROADCAST_TYPE_HIGHLIGHT_SELECTION);
		}
		else{
			this->hintFocusKnot();
		}
		eve.SetNeedRedraw();
		return 1;
	}
	//
	else if(eve.GetEventSource() == MyGenericEvent::MOUSE_KEY
		&& eve.GetEventKeyState() == MyGenericEvent::KEY_DOWN
		&& eve.GetEventMouseKey() == MyGenericEvent::MOUSE_KEY_LEFT
		&& this->GetKnot(0)->GetSelectionLevel() == 1){
		// same thing as hover, but it is selection rather than hover
		MyKnot* knot = mView->GetKnotAt(eve.GetMouseX(), eve.GetMouseY());
		if(knot && knot->GetSelectionLevel()>0){
			//knot->SetSelectionLevel(1);
			MyGenericNode* sel = mView->MakeSeeWhat(eve.GetMouseX(), eve.GetMouseY(), knot);
			if(knot->DoIncludeSelection(sel)){
				MySelectionEvent& seleve = dynamic_cast<MySelectionEvent&>(eve);
				seleve.DeriveActionFromSelection(MySelectionEvent::SEL_DELETE, sel, this);
				knot->RemoveSelection(sel);
				sel->DestoryAll();
				SafeFreeObject(sel);
				//knot->ReplaceSelection(sel);
				knot->BroadCastMessage(MyKnot::BROADCAST_TYPE_HIGHLIGHT_SELECTION);
				eve.SetNeedRedraw();
			}
			else{
				knot->AddSelection(sel);
				//knot->ReplaceSelection(sel);
				knot->BroadCastMessage(MyKnot::BROADCAST_TYPE_HIGHLIGHT_SELECTION);
				eve.SetNeedRedraw();
				MySelectionEvent& seleve = dynamic_cast<MySelectionEvent&>(eve);
				seleve.DeriveActionFromSelection(MySelectionEvent::SEL_ADD, sel, this);
			}
			//knot->SetSelectionLevel(0);
		}
		if(!knot && this->GetNumKnots()>0){
			if(this->GetKnot(0)->GetSelectionLevel()>0 && this->GetKnot(0)->GetNumSelections() >0){
				this->GetKnot(0)->ReplaceSelection(0);
				this->GetKnot(0)->BroadCastMessage(MyKnot::BROADCAST_TYPE_HIGHLIGHT_SELECTION);
				MySelectionEvent& seleve = dynamic_cast<MySelectionEvent&>(eve);
				seleve.DeriveActionFromSelection(MySelectionEvent::SEL_REPLACE, 0, this);
			}
		}
		return 1;
	}
	/*
	// hover event
	else if (eve.GetEventSource() == MyGenericEvent::MOUSE_MOVE
		&& eve.GetEventKeyState() == MyGenericEvent::KEY_UP){
		MyKnot* knot = mView->GetKnotAt(eve.GetMouseX(), eve.GetMouseY());
		if(knot && knot->GetSelectionLevel()>0){
			//knot->SetSelectionLevel(1);
			MyGenericNode* sel = mView->MakeSeeWhat(eve.GetMouseX(), eve.GetMouseY(), knot);
			if(knot->DoIncludeSelection(sel)){
				sel->DestoryAll();
				SafeFreeObject(sel);
				eve.SetNeedRedraw();
			}
			else{
				knot->ReplaceSelection(sel);
				knot->BroadCastMessage(MyKnot::BROADCAST_TYPE_HIGHLIGHT_SELECTION);
				MySelectionEvent& seleve = dynamic_cast<MySelectionEvent&>(eve);
				seleve.DeriveActionFromSelection(MySelectionEvent::SEL_LOOKUP, sel, this);
			}
			//knot->SetSelectionLevel(0);
		}
		if(!knot && this->GetNumKnots()>0){
			if(this->GetKnot(0)->GetSelectionLevel()>0 && this->GetKnot(0)->GetNumSelections() >0){
				this->GetKnot(0)->ReplaceSelection(0);
				this->GetKnot(0)->BroadCastMessage(MyKnot::BROADCAST_TYPE_HIGHLIGHT_SELECTION);
				MySelectionEvent& seleve = dynamic_cast<MySelectionEvent&>(eve);
				seleve.DeriveActionFromSelection(MySelectionEvent::SEL_LOOKUP, 0, this);
			}
		}
			
		return 1;

	}
	*/
	else{
		int nHandle = 0;
		if(mFocusKnot){
			nHandle += mFocusKnot->EventHandler(eve);
		}
		for(int i = 0;i<mLayoutManager.GetNumKnots();i++){
			MyKnot* knot = mKnotContainer.GetKnot(i);
			if(knot != mFocusKnot){
				nHandle += knot->EventHandler(eve);
			}
		}
		Clamp(nHandle,0,1);
		return nHandle;
	}
}

void MyScene::Build(){
	mLayoutManager.Update();
}

MyKnot* MyScene::GetKnot(int i){
	return mKnotContainer.GetKnot(i);
}
const MyKnot* MyScene::GetKnot(int i) const{
	return mLayoutManager.GetKnot(i);
}
int MyScene::GetNumKnots() const{
	return mKnotContainer.GetNumberKnots();
}

/*
int MyScene::keyReleaseEventHandler(unsigned char, int, int){
};
int MyScene::keyPressEventHandler(unsigned char, int, int){
};
int MyScene::mousePressEventHandler(int, int , int){
};

int MyScene::mouseReleaeEventHandler(int, int , int){
};

int MyScene::mouseMoveEventHandler(int, int){
};
*/
void MyScene::hintFocusKnot(){
	for(int i = 0;i<mLayoutManager.GetNumKnots();i++){
		mKnotContainer.GetKnot(i)->SetDrawBoundingBox(false);
		mKnotContainer.GetKnot(i)->ReadyToListen();
	}
	if(mFocusKnot){
		//mFocusKnot->SetDrawBoundingBox();
		mFocusKnot->StartToAnswer();
	}
}