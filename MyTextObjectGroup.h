#pragma once

#include "MyTextObject.h"
#include "MyArray.h"

class MyTextObjectGroup
{
public:
	MyTextObjectGroup(void);
	~MyTextObjectGroup(void);

	void SetStrings(const MyArrayStr& strs);
	int GetNumString() const;
	MyString GetString(int idx) const;
	void Build();

	MyTextObject& GetTextObject(int idx);

protected:
	MyArrayStr mStrings;
	MyArrayi mTextures;
	MyArray<MyTextObject> mTextObjects;
};

