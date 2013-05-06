#include "Visualizer.h"

namespace vis{

Visualizer::Visualizer(void){

	displayWindow = new sf::Window(sf::VideoMode(800,600),"Visualization");
	initGL();
	string pointV = string("point.vert");
	string pointF = string("point.frag");

	shader = Shader(pointV,pointF);

	GLfloat mdlA[16] = {1.0, 0.0, 0.0, 0.0,
						0.0, 1.0, 0.0, 0.0,
						0.0, 0.0, 1.0, 0.0,
						0.0, 0.0, 0.0, 1.0};
	mdl = new GLfloat[16];
	
	sphereModel = new Model();

	//copy modulating matrix to the dynamically allocated one
	//there is probably a better way to do this...
	for(int i=0; i < 16; i++){
		mdl[i] = mdlA[i];
	}
}

Visualizer::~Visualizer(void){

}

void Visualizer::addPoint(GLfloat x, GLfloat y, GLfloat z){
	plottedPoints.push_back(Object(shader.programRef,sphereModel,x,y,z));
}


void Visualizer::mainLoop(){
	bool running = true;

	cout << "visualizer started..." << endl;

	while(running){
		sf::Event event;

		while(displayWindow->pollEvent(event)){
			if(event.type == sf::Event::Closed){
				running = false;
			}else if(event.type == sf::Event::Resized){
				glViewport(0,0,event.size.width, event.size.height);
			}
		}
		cam.updatePosition();

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