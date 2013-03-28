// object.h, Contains class definition of object

#include <iostream>
#include <opencv2/core/core.hpp>
#include "Prediction\StateSpaceModel.h"
#include <list>

#ifndef OBJECT_H
#define OBJECT_H

class Object
{
public:
	Object() {x = 0, y = 0, dx = 0, dy = 0, xHat = 0, yHat = 0, width = 0, height = 0, id = -1;}
	Object(int x, int y, float dx, float dy, float xHat = 0, float yHat = 0, int width = 0, int height = 0, int id = -1);
	Object(cv::Rect boundingBox, float dx = 0, float dy = 0, float xHat = 0, float yHat = 0, int id = -1);

	int id, x, y, width, height;
	float xHat, yHat;
	float dx, dy;

	float positionUncertantyX, positionUncertantyY, widthUncertanty, heightUncertanty;

	Prediction::StateSpaceModel model;

	std::list<Object*> children, parents;
	bool isLost, isHidden, isParent, isChild, isDecided;

	
	cv::Rect boundingBox;
		
	friend std::ostream & operator<< (std::ostream & o, Object & obj);
	void info();
	bool operator==(const Object & other) { return id == other.id; }
	bool operator<(const Object & other) { return id < other.id; }

	float containedAreaQuotient(Object & other);
	float intervalOverlap(float x1, float x2, float y1, float y2);

	void addPositionUncertainty(float positionErrorX, float positionErrorY) { this->positionUncertantyX += positionErrorX; this->positionUncertantyY += positionErrorY; }
	void addSizeUncertainty(float widthError, float heightError) { this->widthUncertanty += widthError; this->heightUncertanty += heightError; }
};
#endif