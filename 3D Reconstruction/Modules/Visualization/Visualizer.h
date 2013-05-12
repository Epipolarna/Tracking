#pragma once

#include <GL\glew.h>
#include <GL\GL.h>
#include <GL\GLU.h>
#include <SFML/Window.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/OpenGL.hpp>
#include <opencv2\core\core.hpp>
#include <iomanip>
#include <assert.h>

#include <string>
#include <vector>

#include "Shader.h"
#include "VisualisationDisplayCameraClass.h"
#include "Model.h"
#include "Object.h"
#include "Texture.h"


using namespace std;
using namespace cv;

namespace vis{

class Visualizer{

public:
	Visualizer(void);
	~Visualizer(void);

	void addPoint(GLfloat x, GLfloat y, GLfloat z);
	void addPoint(Vec3f, Mat texture);
	void addCamera(Mat externalParameters);

	void mainLoop();
public:
	
	void initGL(void);
	void uploadGeometry(void);
	void generateSphere(void);
	void redraw(void);
	GLuint* imageToTexture(Mat texture);

	//variables
	Shader shader;
	sf::RenderWindow * displayWindow;
	vector<Object> plottedPoints;
	vector<Object> cameras;
	GLfloat* mdl;
	VisualisationDisplayCameraClass cam;

	Model* sphereModel;
	Model* boxModel;

};

}

