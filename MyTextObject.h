#pragma once

#include "MyString.h"

class MyTextObject
{
public:
	MyTextObject(void);
	~MyTextObject(void);

	void Show();
	void SetString(const MyString& str);
	MyString GetString() const;
	
	void SetTextureIndex(int tex);
	int GetTextTextureIndex() const;
	void Build();

	int GetWidth() const;
	int GetHeight() const;
	int GetTextureWidth() const;
	int GetTextureHeight() const;

protected:
	MyString mString;
	int mTextTexture;
	int mWidth;
	int mHeight;

	int mTextureWidth;
	int mTextureHeight;
};

