#pragma once
#include <GL\glew.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include <opencv2\core\core.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML\Window\Keyboard.hpp>
#include <SFML\Window\Mouse.hpp>

#include <iostream>

class VisualisationDisplayCameraClass{
public:
	VisualisationDisplayCameraClass();

	void lookAtUpdate(float dt);
	void lookAtUpload(GLuint program);

	void updatePosition(void);

	void moveForward(float dt);
	void moveBackward(float dt);
	void moveLeft(float dt);
	void moveRight(float dt);
	void moveUp(float dt);
	void moveDown(float dt);
	void lookUp(float dt);
	void lookDown(float dt);
	void lookLeft(float dt);
	void lookRight(float dt);

	void mouseMovement(float dt, sf::Vector2i pos);
	void updateCameraRotation(void);

	cv::Vec3f getLookAtDirection() {return normalize(lookAtVector - position);};

	GLfloat turningRadius;

	int wWidth, wHeight;

	cv::Vec3f position, lookAtVector, upVector;
	cv::Vec3f rotAngles;

	cv::Mat lookAtMatrix;
	cv::Mat cameraRotation;
	GLfloat sensitivity, arrowSensitivity, movementSpeed;
};

