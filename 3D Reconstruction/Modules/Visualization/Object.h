#pragma once
#include <opencv2\core\core.hpp>
#include "Model.h"

using namespace cv;

namespace vis{

class Object
{
public:
	Object(GLuint program, Model* nm, float x, float y, float z);
	~Object(void);

	void draw(void);
	void updateMatrices();

	GLuint program;
	Vec3f scale;
	Vec3f position;

	Vec3f rotAngles;

	Mat scaleTrans;
	Mat totalRot;
	Mat rotX;
	Mat rotY;
	Mat rotZ;

	Model* m;


};

};