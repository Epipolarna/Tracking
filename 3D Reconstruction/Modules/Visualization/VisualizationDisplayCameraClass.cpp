#include "VisualisationDisplayCameraClass.h"

VisualisationDisplayCameraClass::VisualisationDisplayCameraClass()
{
	position		= cv::Vec3f(0, 0, 0);
	lookAtVector	= cv::Vec3f(0, 0, 10);
	upVector		= cv::Vec3f(0, 1, 0);

	sensitivity = 0.025f;
	arrowSensitivity = 0.4f;
	movementSpeed = 10.0f;
}

void VisualisationDisplayCameraClass::updatePosition(void){
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space)){
		moveUp(0.01);
	}
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)){
		moveDown(0.01);
	}
	
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)){
		moveForward(0.01);
	}
	
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)){
		moveBackward(0.01);
	}
	
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)){
		moveLeft(0.01);
	}

	if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)){
		moveRight(0.01);
	}
}

void VisualisationDisplayCameraClass::lookAtUpdate(float dt){

	cv::Vec3f n = cv::normalize(position - lookAtVector);

	cv::Vec3f u = cv::normalize(upVector.cross(n));

	cv::Vec3f v = n.cross(u);

	GLfloat r[16] = {   u[0], u[1], u[2], 0.0,
                        v[0], v[1], v[2], 0.0,
                        n[0], n[1], n[2], 0.0,
                        0.0, 0.0, 0.0, 1.0};
	cv::Mat rot(4, 4, CV_32FC1, r);

	GLfloat t[16] = {   1.0, 0.0, 0.0, -position[0],
                        0.0, 1.0, 0.0, -position[1],
                        0.0, 0.0, 1.0, -position[2],
                        0.0, 0.0, 0.0, 1.0};
	cv::Mat trans(4, 4, CV_32FC1, t);

	lookAtMatrix = rot*trans;
}

void VisualisationDisplayCameraClass::lookAtUpload(GLuint program){
	glUniformMatrix4fv(glGetUniformLocation(program, "camMatrix"), 1, GL_TRUE, lookAtMatrix.ptr<GLfloat>());
}

void VisualisationDisplayCameraClass::moveUp(float dt)
{
	//std::cout << "moveUp" << std::endl;
	cv::Vec3f prevPos = position;
	position += cv::normalize(upVector)*movementSpeed*dt;
	lookAtVector += position - prevPos;
	//std::cout << position - prevPos << std::endl;
}

void VisualisationDisplayCameraClass::moveDown(float dt)
{
	//std::cout << "moveDown" << std::endl;
	cv::Vec3f prevPos = position;
	position -= cv::normalize(upVector)*movementSpeed*dt;
	lookAtVector += position - prevPos;
	//std::cout << position - prevPos << std::endl;
}

void VisualisationDisplayCameraClass::moveForward(float dt)
{
	//std::cout << "W" << std::endl;
	position += cv::normalize(lookAtVector - position)*movementSpeed*dt;
	lookAtVector += cv::normalize(lookAtVector - position)*movementSpeed*dt;
	//std::cout << position << std::endl;
}
void VisualisationDisplayCameraClass::moveBackward(float dt)
{
	//std::cout << "S" << std::endl;
	position -= cv::normalize(lookAtVector - position)*movementSpeed*dt;
	lookAtVector -= cv::normalize(lookAtVector - position)*movementSpeed*dt;
	//std::cout << position << std::endl;
}
void VisualisationDisplayCameraClass::moveLeft(float dt)
{
	//std::cout << "A" << std::endl;
	position -= cv::normalize( (lookAtVector - position).cross(upVector) )*movementSpeed*dt;
	lookAtVector -= cv::normalize( (lookAtVector - position).cross(upVector) )*movementSpeed*dt;
	//std::cout << position << std::endl;
}
void VisualisationDisplayCameraClass::moveRight(float dt)
{
	//std::cout << "D" << std::endl;
	position += cv::normalize( (lookAtVector - position).cross(upVector) )*movementSpeed*dt;
	lookAtVector += cv::normalize( (lookAtVector - position).cross(upVector) )*movementSpeed*dt;
	//std::cout << position << std::endl;
}


void VisualisationDisplayCameraClass::xLook(float dt, int dx)
{
	//std::cout << "xlook " << dx << std::endl;
	cv::Vec3f lookAtDirection = lookAtVector - position;
	GLfloat length = (GLfloat)cv::norm(lookAtDirection);
	cv::Vec3f helpVector = cv::normalize(lookAtDirection.cross(upVector));

	lookAtVector = position + cv::normalize(lookAtDirection + sensitivity*helpVector*dx)*length;
	//upVector = cv::normalize(helpVector.cross(lookAtVector - position));
}

void VisualisationDisplayCameraClass::yLook(float dt, int dy)
{
	//std::cout << "ylook " << dy << std::endl;
	cv::Vec3f lookAtDirection = lookAtVector - position;
	GLfloat length = (GLfloat)cv::norm(lookAtDirection);
	cv::Vec3f helpVector = lookAtDirection.cross(upVector);

	lookAtVector = position + cv::normalize(lookAtDirection + sensitivity*upVector*dy)*length;
	//upVector = cv::normalize(helpVector.cross(lookAtVector - position));
}