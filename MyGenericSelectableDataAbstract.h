#pragma once

#include "MyGenericNode.h"

/*
	this class aims to provide a generic selection method.
	selection is defined basically as a hierarchy.
	Only the correct sub class can understand and correctly
	interpret the selection
*/
class MyGenericSelectableDataAbstract
{
public:
	MyGenericSelectableDataAbstract(void);
	~MyGenericSelectableDataAbstract(void);

	// feed a selection to this class,
	// must be destory outside this class
	virtual void SetSelection(const MyGenericNode* selection);
	virtual void CancelSelection();
	virtual const MyGenericNode* GetSelection() const;

	// implement you own method.
	// like highlight etc
	virtual void SetEmphasizeSelection(bool emphasize = true);

	// return another MyGenericSelectableDataAbstract class pointer
	// that is indicated by the selection
	// remember to destory it
	// memory leak alert
	static MyGenericSelectableDataAbstract* MakeTranslatedSelection(const MyGenericNode* selection);
	
	// remember to destory it
	// memory leak alert
	static MyGenericNode* MakeGenericSelection(const MyGenericSelectableDataAbstract* sub);

protected:
	const MyGenericNode* mSelection;
	bool mEmphasizeSelection;
};

