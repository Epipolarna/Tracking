#include "Object.h"


namespace vis{

Object::Object(GLuint p, Model* nm, float nx, float ny, float nz){
	program = p;
	position = Vec3f(nx,ny,nz);
	scale = Vec3f(1.0,1.0,1.0);
	rotAngles = Vec3f(0,0,0);
	m = nm;
}


Object::~Object(void)
{
}


void Object::draw(void){
	updateMatrices();
	glUniformMatrix4fv(glGetUniformLocation(program, "scaleTrans"), 1, GL_TRUE, scaleTrans.ptr<GLfloat>());
	glUniformMatrix4fv(glGetUniformLocation(program, "objectRot"), 1, GL_TRUE, totalRot.ptr<GLfloat>());
	m->draw(program);
}


void Object::updateMatrices(){
	GLfloat	scaleTransData[] = {	scale(0), 0.0f,	0.0f,	position(0),
									0.0f,	scale(1),	0.0f,	position(1),
									0.0f,	0.0f,	scale(2), position(2),
									0.0f,	0.0f,	0.0f,	1.0f };

	scaleTrans = cv::Mat(4, 4, CV_32FC1, scaleTransData).clone();
	
	//rotation matrices
	GLfloat	rotXData[] = {	1.0f, 0.0f,			0.0f,		0.0f,
							0.0f, cos(rotAngles(0)),	-sin(rotAngles(0)),	0.0f,
							0.0f, sin(rotAngles(0)),	cos(rotAngles(0)),	0.0f,
							0.0f, 0.0f,			0.0f,		1.0f }; 
	rotX = cv::Mat(4, 4, CV_32FC1, rotXData).clone();

	GLfloat rotYData[] = {	cos(rotAngles(1)),	0.0f,	sin(rotAngles(1)),	0.0f,
							0.0f,		1.0f,	0.0f,		0.0f,
							-sin(rotAngles(1)),	0.0f,	cos(rotAngles(1)),	0.0f,
							0.0f,		0.0f,	0.0f,		1.0f };
	rotY = cv::Mat(4, 4, CV_32FC1, rotYData).clone();

	GLfloat rotZData[] = {	cos(rotAngles(2)),	-sin(rotAngles(2)),	0.0f, 0.0f,
							sin(rotAngles(2)),	cos(rotAngles(2)),	0.0f, 0.0f,
							0.0f,		0.0f,		1.0f, 0.0f,
							0.0f,		0.0f,		0.0f, 1.0f }; 
	rotZ = cv::Mat(4, 4, CV_32FC1, rotZData).clone();


	totalRot = rotX*rotY*rotZ;
}
}