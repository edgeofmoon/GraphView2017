#pragma once

class MyTextureHelper
{
public:
	MyTextureHelper(void);
	virtual ~MyTextureHelper(void);

	virtual void GenerateTexture();
	virtual void ActiveTexture() const;
	virtual void DeactiveTexture() const;
	virtual void DeleteTexture();

protected:
	unsigned int mTexture;
};

class MyHighlightTextureHelper
	: public MyTextureHelper
{
public:
	virtual void GenerateTexture();
};

