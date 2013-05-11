#include "Visualizer.h"

namespace vis{


Visualizer::Visualizer(void){

	displayWindow = new sf::Window(sf::VideoMode(800,800),"Visualization");
	initGL();
	string pointV = string("point.vert");
	string pointF = string("point.frag");

	cam.wHeight = 800;
	cam.wWidth = 800;
	cam.turningRadius = 10.0;

	shader = Shader(pointV,pointF);

	GLfloat mdlA[16] = {1.0, 0.0, 0.0, 0.0,
						0.0, 1.0, 0.0, 0.0,
						0.0, 0.0, 1.0, 0.0,
						0.0, 0.0, 0.0, 1.0};
	mdl = new GLfloat[16];
	
	sphereModel = new Model(sphere);
	boxModel = new Model(cube);

	//copy modulating matrix to the dynamically allocated one
	//there is probably a better way to do this...
	for(int i=0; i < 16; i++){
		mdl[i] = mdlA[i];
	}
}

Visualizer::~Visualizer(void){

}

void Visualizer::addCamera(Mat externalParameters){
	
	Mat C = externalParameters;

	Mat R = C.rowRange(Range(0,3)).colRange(Range(0,3));
	Mat t = C.rowRange(Range(0,3)).colRange(Range(3,4));

	Mat pos = -R.t()*t;

	float x = pos.at<double>(0,0);
	float y = pos.at<double>(1,0);
	float z = pos.at<double>(2,0);

	/*
	float x = externalParameters.at<double>(0,3);
	float y = externalParameters.at<double>(1,3);
	float z = externalParameters.at<double>(2,3);
	*/

	Object newCamera = Object(shader.programRef,boxModel,x,y,z);

	newCamera.scale = Vec3f(2,2,0.5);
	newCamera.totalRot = externalParameters(Rect(0,0,2,2));

	cameras.push_back(newCamera);
}

void Visualizer::addPoint(GLfloat x, GLfloat y, GLfloat z){
	plottedPoints.push_back(Object(shader.programRef,sphereModel,x,y,z));
}

void Visualizer::addPoint(Vec3f pos, Mat texture){
	Object newPoint = Object(shader.programRef,sphereModel,pos[0],pos[1],pos[2]);

	Texture *tex = new Texture();
	tex->dataFromMatrix(texture);
	newPoint.tex = tex;
	plottedPoints.push_back(newPoint);
}

void Visualizer::mainLoop(){
	bool running = true;
	int dx, dy; 
	dx = dy = 0;
	cout << "visualizer started..." << endl;

	while(running){
		sf::Event event;

		while(displayWindow->pollEvent(event)){
			if(event.type == sf::Event::Closed){
				running = false;
			}else if(event.type == sf::Event::Resized){
				glViewport(0,0,event.size.width, event.size.height);
			}else if(event.type == sf::Event::MouseMoved){
				
			}else if(event.type == sf::Event::KeyPressed){
				if(event.key.code == sf::Keyboard::Escape)
					running = false;
			}
		}
		dx = sf::Mouse::getPosition(*displayWindow).x - displayWindow->getSize().x / 2;
		dy = sf::Mouse::getPosition(*displayWindow).y - displayWindow->getSize().y / 2;
		dy = -dy;
		sf::Mouse::setPosition(sf::Vector2i(displayWindow->getSize().x / 2, displayWindow->getSize().y / 2),  *displayWindow);
		cam.updatePosition(dx, dy);
		dx = dy = 0;
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		redraw();
		displayWindow->display();
	}
}

void Visualizer::redraw(){
#define nearFrustum 1.0
#define farFrustum 500.0
#define right 1.0
#define left -1.0
#define top 1.0
#define bottom -1.0
GLfloat frustumMatrix[] = {		2.0f*nearFrustum/(right-left), 0.0f,					(right+left)/(right-left),	0.0f,
								0.0f,					2.0f*nearFrustum/(top-bottom),	(top+bottom)/(top-bottom),	0.0f,
								0.0f,					0.0f,					-((float)farFrustum + (float)nearFrustum)/((float)farFrustum - (float)nearFrustum),	-2.0f*(float)farFrustum*(float)nearFrustum/((float)farFrustum - (float)nearFrustum),
								0.0f,					0.0f,					-1.0f,						0.0f };
	static int lastError = -1;
	glUseProgram(shader.programRef);

	//cout << "shader program ref is: " << shader.programRef << endl;
	//cout << "uniform location " << glGetUniformLocation(shader->programRef, "mdlMatrix") << endl;

	glUniformMatrix4fv(glGetUniformLocation(shader.programRef,"mdlMatrix"), 1, GL_TRUE, mdl);
	glUniformMatrix4fv(glGetUniformLocation(shader.programRef,"projMatrix"), 1, GL_TRUE, frustumMatrix);
	cam.lookAtUpdate(1.0);
	cam.lookAtUpload(shader.programRef);


	for(vector<Object>::iterator it = plottedPoints.begin(); it != plottedPoints.end(); ++it){
		it->draw();
	}

	for(vector<Object>::iterator it = cameras.begin(); it != cameras.end(); ++it){
		it->draw();
	}
	
	int error = glGetError();
	if(error != 0 || error != lastError){
		cout << "opengl error is: 0x" << hex << error << endl;
		lastError = error;
	}
}

void Visualizer::initGL(void){

	if(GLEW_OK != glewInit()){
		cout << "glew init failed " << endl;
	}

	glClearColor(0.0,0.0,0.0,1.0);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
}

}