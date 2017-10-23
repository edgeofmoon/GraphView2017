#include "MyTextureHelper.h"
#include <cmath>
#define TEXTURESIZE 8
#define BANDWIDTH 2
#define STRIPWIDTH 2

#define TEXTURESIZE_2 32

#include "gl/glut.h"
//#include "gl/glew.h"

MyTextureHelper::MyTextureHelper(void)
{
}


MyTextureHelper::~MyTextureHelper(void){
}

void MyTextureHelper::GenerateTexture(){
		unsigned char data[TEXTURESIZE][TEXTURESIZE][4];
		for(int i = 0;i<TEXTURESIZE;i++){
			if(i%(BANDWIDTH+STRIPWIDTH)<BANDWIDTH){
				// draw band
				for(int j = 0;j<TEXTURESIZE;j++){
					data[i][j][0] = 255;
					data[i][j][1] = 255;
					data[i][j][2] = 255;
					data[i][j][3] = 255;
				}
			}
			else{
				// draw strip
				for(int j = 0;j<TEXTURESIZE;j++){
					data[i][j][0] = 0;
					data[i][j][1] = 0;
					data[i][j][2] = 0;
					data[i][j][3] = 255;
				}
			}
		}
		// make texutre
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glGenTextures(1, &mTexture);
		glBindTexture(GL_TEXTURE_2D, mTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TEXTURESIZE, 
					TEXTURESIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, 
					data);
		//glGenerateMipmap(GL_TEXTURE_2D); 

		//GLenum error = glGetError();

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
}

void MyTextureHelper::ActiveTexture() const{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, mTexture);
}

void MyTextureHelper::DeactiveTexture() const{
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
}

void MyTextureHelper::DeleteTexture(){
	glDeleteTextures(1,&mTexture);
}

void MyHighlightTextureHelper::GenerateTexture(){
		unsigned char data[TEXTURESIZE_2][TEXTURESIZE_2][4];
		int maxDist = (TEXTURESIZE_2/2)*(TEXTURESIZE_2/2);
		for(int i = 0;i<TEXTURESIZE_2;i++){
			int dx = i-TEXTURESIZE_2/2;
			for(int j = 0;j<TEXTURESIZE_2;j++){
				int dy = j-TEXTURESIZE_2/2;
				int dist = dx*dx+dy*dy;
				if(dist>maxDist){
					data[i][j][0] = 0;
					data[i][j][1] = 0;
					data[i][j][2] = 0;
					data[i][j][3] = 0;
				}
				else{
					float ratio = 1-sqrtf(dist/(float)maxDist);
					int c = 125+0.5;
					data[i][j][0] = c;
					data[i][j][1] = c;
					data[i][j][2] = c;
					data[i][j][3] = ratio*255;
				}
			}
		}
		// make texutre
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glGenTextures(1, &mTexture);
		glBindTexture(GL_TEXTURE_2D, mTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TEXTURESIZE_2, 
					TEXTURESIZE_2, 0, GL_RGBA, GL_UNSIGNED_BYTE, 
					data);
		//glGenerateMipmap(GL_TEXTURE_2D); 

		//GLenum error = glGetError();

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
}