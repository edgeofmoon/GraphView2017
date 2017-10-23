#include "MyTextObjectGroup.h"
#include "MyPrimitiveDrawer.h"

MyTextObjectGroup::MyTextObjectGroup(void)
{
	mTextures.clear();
}


MyTextObjectGroup::~MyTextObjectGroup(void)
{
	mTextObjects.clear();
}

void MyTextObjectGroup::SetStrings(const MyArrayStr& strs){
	mStrings = strs;
}

int MyTextObjectGroup::GetNumString() const{
	return mStrings.size();
}

MyString MyTextObjectGroup::GetString(int idx) const{
	return mStrings[idx];
}

void MyTextObjectGroup::Build(){
	MyPrimitiveDrawer::DeleteTextures(mTextures);
	mTextObjects.clear();
	mTextures = MyPrimitiveDrawer::GenerateTextures(mStrings.size());
	mTextObjects.resize(mStrings.size());
	for(int i = 0;i<mStrings.size();i++){
		mTextObjects[i].SetString(mStrings[i]);
		mTextObjects[i].SetTextureIndex(mTextures[i]);
		mTextObjects[i].Build();
	}
}

MyTextObject& MyTextObjectGroup::GetTextObject(int idx){
	return mTextObjects[idx];
}