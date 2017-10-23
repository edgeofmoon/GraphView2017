#include "MyAntiAliasingView.h"
#include "MyPrimitiveDrawer.h"
#include "gl\glew.h"

// if this is too large
// glRenderbufferStorage gives error 1281: invalid value
#define DETAIL_X 2
#define DETAIL_Y 2
MyAntiAliasingView::MyAntiAliasingView(void)
{
}


MyAntiAliasingView::~MyAntiAliasingView(void)
{
}

void MyAntiAliasingView::Show(){
	static int mTextureWidth = 1;
	while(mTextureWidth < mWidth*DETAIL_X) mTextureWidth*=2;
	static int mTextureHeight = 1;
	while(mTextureHeight < mHeight*DETAIL_Y) mTextureHeight*=2;

	static int mTextTexture = -1;
	if(!MyPrimitiveDrawer::IsTexture(mTextTexture)){
		mTextTexture = MyPrimitiveDrawer::GenerateTexture();
	}

	bool init = false;
	if(!init){
		glewInit();
		init = true;
	}

	GLuint fbo, render_buf;
	glGenFramebuffers(1,&fbo);
	glGenRenderbuffers(1,&render_buf);
	glBindRenderbuffer(GL_RENDERBUFFER, render_buf);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, mTextureWidth, mTextureHeight);
	glBindFramebuffer(GL_FRAMEBUFFER,fbo);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, render_buf);

	//Before drawing
	glBindFramebuffer(GL_RENDERBUFFER,fbo);
	MyPrimitiveDrawer::ClearFrameBuffer();
	//MyPrimitiveDrawer::PushMatrix();
	//MyPrimitiveDrawer::PushProjectionMatrix();
	MyVec4i viewport = MyPrimitiveDrawer::GetViewport();
	MyPrimitiveDrawer::SetViewport(MyVec4i(0,0, mWidth*DETAIL_X, mHeight*DETAIL_Y));
	//MyPrimitiveDrawer::SetToByPixelMatrix(mWidth, mHeight, 0, 1000);
	MyMultiView::Show();
	MyPrimitiveDrawer::SetViewport(viewport);
	//MyPrimitiveDrawer::PopMatrix();
	//MyPrimitiveDrawer::PopProjectionMatrix();

	//after drawing
	//glReadBuffer(GL_BACK);
	MyPrimitiveDrawer::EnableTexture2D();
	MyPrimitiveDrawer::BindTexture2D(mTextTexture);
	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
	MyPrimitiveDrawer::SpecifyTextureImage2D(mTextureWidth, mTextureHeight);
	MyPrimitiveDrawer::AutoSpecifyTexutreParamters();
	//glCopyTexSubImage2D (GL_TEXTURE_2D, 0, 0, 0, 0, 0, mTextureWidth, mTextureHeight);
	MyPrimitiveDrawer::CopyToTextureImage2D(0,0,mTextureWidth,mTextureHeight);
	//MyPrimitiveDrawer::GenerateMipMap2D();
	MyPrimitiveDrawer::UnbindTexture2D(mTextTexture);
	MyPrimitiveDrawer::DisableTexture2D();

	//At deinit:
	glDeleteFramebuffers(1,&fbo);
	glDeleteRenderbuffers(1,&render_buf);

	// Return to onscreen rendering:
	glBindFramebuffer(GL_RENDERBUFFER,0);

	// show
	MyPrimitiveDrawer::ClearFrameBuffer();
	MyPrimitiveDrawer::EnableTexture2D();
	MyPrimitiveDrawer::BindTexture2D(mTextTexture);
	//MyPrimitiveDrawer::EnableAlplaBlending();
	
	MyPrimitiveDrawer::PushMatrix();
	MyPrimitiveDrawer::PushProjectionMatrix();
	MyPrimitiveDrawer::LoadModelViewMatrix(&MyMatrixf::IdentityMatrix());
	MyPrimitiveDrawer::LoadProjectionMatrix(&MyMatrixf::IdentityMatrix());

	float widthRatio = mWidth*DETAIL_X/(float)mTextureWidth;
	float heightRatio = mHeight*DETAIL_Y/(float)mTextureHeight;
	
	MyPrimitiveDrawer::Color(MyColor4f(1,1,1,1));
	MyPrimitiveDrawer::BeginQuads();
	MyPrimitiveDrawer::TextureCoordinate(MyVec2f(0,0));
	MyPrimitiveDrawer::Vertex(MyVec2f(-1,-1));
	MyPrimitiveDrawer::TextureCoordinate(MyVec2f(widthRatio,0));
	MyPrimitiveDrawer::Vertex(MyVec2f(1,-1));
	MyPrimitiveDrawer::TextureCoordinate(MyVec2f(widthRatio,heightRatio));
	MyPrimitiveDrawer::Vertex(MyVec2f(1,1));
	MyPrimitiveDrawer::TextureCoordinate(MyVec2f(0,heightRatio));
	MyPrimitiveDrawer::Vertex(MyVec2f(-1,1));
	MyPrimitiveDrawer::EndPrimitive();

	MyPrimitiveDrawer::PopMatrix();
	MyPrimitiveDrawer::PopProjectionMatrix();
	MyPrimitiveDrawer::DisableAlplaBlending();
	MyPrimitiveDrawer::UnbindTexture2D(mTextTexture);
	MyPrimitiveDrawer::DisableTexture2D();
}