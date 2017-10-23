#include "MyGenericSelectableDataAbstract.h"


MyGenericSelectableDataAbstract::MyGenericSelectableDataAbstract(void)
{
	mSelection = 0;
	mEmphasizeSelection = false;
}


MyGenericSelectableDataAbstract::~MyGenericSelectableDataAbstract(void)
{
}

void MyGenericSelectableDataAbstract::SetSelection(const MyGenericNode* selection){
	mSelection = selection;
}

void MyGenericSelectableDataAbstract::CancelSelection(){
	mSelection = 0;
}

const MyGenericNode* MyGenericSelectableDataAbstract::GetSelection() const{
	return mSelection;
}

void MyGenericSelectableDataAbstract::SetEmphasizeSelection(bool emphasize){
	mEmphasizeSelection = emphasize;
}

MyGenericSelectableDataAbstract* MyGenericSelectableDataAbstract::MakeTranslatedSelection(const MyGenericNode* selection){
	return 0;
}

MyGenericNode* MyGenericSelectableDataAbstract::MakeGenericSelection(const MyGenericSelectableDataAbstract* sub){
	return 0;
}