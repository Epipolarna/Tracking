#pragma once
#include <GL\glew.h>
#include <GL\GL.h>
#include <GL\GLU.h>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <opencv2\core\core.hpp>

#include <assert.h>
#include <iostream>
#include <iomanip>

using namespace cv;
using namespace std;

namespace vis{
class Texture{
public:
	Texture();
	~Texture();

	void dataFromMatrix(Mat m);
	void upload();

	GLubyte *textureData;

	int xSize,ySize;
	GLuint refId;

};
}