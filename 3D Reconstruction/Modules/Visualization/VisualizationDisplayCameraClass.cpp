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

	float movementSpeed = 0.01;

	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space)){
		moveUp(movementSpeed);
	}
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)){
		moveDown(movementSpeed);
	}
	
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)){
		moveForward(movementSpeed);
	}
	
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)){
		moveBackward(movementSpeed);
	}
	
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)){
		moveLeft(movementSpeed);
	}

	if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)){
		moveRight(movementSpeed);
	}

	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Q)){
		moveUp(movementSpeed);
	}

	if(sf::Keyboard::isKeyPressed(sf::Keyboard::E)){
		moveDown(movementSpeed);
	}
}

void VisualisationDisplayCameraClass::lookAtUpdate(float dt){

	mouseMovement(dt,sf::Mouse::getPosition());

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

void VisualisationDisplayCameraClass::lookUp(float dt)
{
	//std::cout << "Up" << std::endl;
	cv::Vec3f lookAtDirection = lookAtVector - position;
	GLfloat length = (GLfloat)cv::norm(lookAtDirection);
	cv::Vec3f helpVector = lookAtDirection.cross(upVector);

	lookAtVector = position + cv::normalize(lookAtDirection + arrowSensitivity*upVector)*length;
	upVector = cv::normalize(helpVector.cross(lookAtVector - position));
}
void VisualisationDisplayCameraClass::lookDown(float dt)
{
	//std::cout << "Down" << std::endl;
	cv::Vec3f lookAtDirection = lookAtVector - position;
	GLfloat length = (GLfloat)cv::norm(lookAtDirection);
	cv::Vec3f helpVector = lookAtDirection.cross(upVector);

	lookAtVector = position + cv::normalize(lookAtDirection - arrowSensitivity*upVector)*length;
	upVector = cv::normalize(helpVector.cross(lookAtVector - position));
}


void VisualisationDisplayCameraClass::mouseMovement(float dt, sf::Vector2i pos){
	position[0] = lookAtVector[0] + turningRadius*cos(2*3.1415*pos.x/wWidth);
	position[2] = lookAtVector[2] + turningRadius*sin(2*3.1415*pos.x/wWidth);

	//something for up and down aswell...
}