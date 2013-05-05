#pragma once
#include <GL\glew.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include <opencv2\core\core.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML\Window\Keyboard.hpp>
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

	void xLook(float dt, int dx);
	void yLook(float dt, int dy);

	cv::Vec3f getLookAtDirection() {return normalize(lookAtVector - position);};

	cv::Vec3f position, lookAtVector, upVector;
	cv::Mat lookAtMatrix;
	GLfloat sensitivity, arrowSensitivity, movementSpeed;
};

