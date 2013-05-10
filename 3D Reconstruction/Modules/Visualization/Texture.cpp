#include "Texture.h"

namespace vis{
	Texture::Texture(){

	}

	Texture::~Texture(){

	}

	void Texture::dataFromMatrix(Mat m){
	xSize = m.rows;
	ySize = m.cols;

	int total = xSize*ySize*3;
	textureData = new GLubyte[total];

	for(int x=0; x < xSize; x++){
		for(int y=0; y < ySize; y++){
			for(int c=0; c < 3; c++){
				textureData[(x*xSize+y)*3+c] = m.at<Vec3b>(x,y)[c];
			}
		}
	}

	upload();

	}

	void Texture::upload(){
		glGenTextures(1, &refId);
		cout << "texture error0 is: 0x" << hex << glGetError() << endl;
		glBindTexture(GL_TEXTURE_2D, refId);
		cout << "texture error1 is: 0x" << hex << glGetError() << endl;
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		cout << "texture error2 is: 0x" << hex << glGetError() << endl;

		glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,xSize,ySize,0,GL_RGB,GL_UNSIGNED_BYTE,textureData);
		cout << "texture error3 is: 0x" << hex << glGetError() << endl;
	}
}