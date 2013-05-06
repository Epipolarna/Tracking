#include "Object.h"


namespace vis{

Object::Object(GLuint p, Model* nm, float nx, float ny, float nz){
	program = p;
	position = Vec3f(nx,ny,nz);
	scale = Vec3f(1.0,1.0,1.0);
	m = nm;
}


Object::~Object(void)
{
}


void Object::draw(void){
	updateMatrices();
	glUniformMatrix4fv(glGetUniformLocation(program, "scaleTrans"), 1, GL_TRUE, scaleTrans.ptr<GLfloat>());
	m->draw(program);
}


void Object::updateMatrices(){
	GLfloat	scaleTransData[] = {	scale(0), 0.0f,	0.0f,	position(0),
									0.0f,	scale(1),	0.0f,	position(1),
									0.0f,	0.0f,	scale(2), position(2),
									0.0f,	0.0f,	0.0f,	1.0f };

	scaleTrans = cv::Mat(4, 4, CV_32FC1, scaleTransData).clone();
}
}