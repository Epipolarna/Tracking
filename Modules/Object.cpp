// object.ccp

//#include "stdafx.h"

#include "Object.h"
#include <iostream>

using namespace std;

Object::Object(int x, int y, float dx, float dy, int xHat, int yHat, int width, int height, int id)
: x(x), y(y), dx(dx), dy(dy), xHat(xHat), yHat(yHat), width(width), height(height), id(id)  
{
	boundingBox.x = x-width/2;
	boundingBox.y = y-height/2;
	boundingBox.width = width;
	boundingBox.height = height;
	lost = false;
}

Object::Object(cv::Rect boundingBox, float dx, float dy, int id)
: boundingBox(boundingBox), dx(dx), dy(dy), id(id) 
{
	x = boundingBox.x + boundingBox.width/2;
	y = boundingBox.y + boundingBox.height/2;
	width = boundingBox.width;
	height = boundingBox.height;
}

std::ostream & operator<< (std::ostream & o, Object & object)
{
	o << "ID: " << object.id << "\n";
	o << "Position: (" << object.x << ", " << object.y << ")" << "\n";
	o << "Dimenson: (" << object.width << ", " << object.height << ")" << "\n";
	o << "Velocity: (" << object.xHat << ", " << object.yHat << ")" << "\n";
	return o;
}

void Object::info()
{
	cout << "ID: " << id << endl;
	cout << "Position: (" << x << ", " << y << ")" << endl;
	cout << "PEstimate: (" << xHat << ", " << yHat << ")" << endl;
	cout << "Dimenson: (" << width << ", " << height << ")" << endl;
	cout << "Velocity: (" << dx << ", " << dy << ")" << endl;
}

float Object::containedAreaQuotient(Object & other)
{
	float w = intervalOverlap(x-width/2, x+width/2, other.x-other.width/2, other.x+other.width/2);
	float h = intervalOverlap(y-height/2, y+height/2, other.y-other.height/2, other.y+other.height/2);
	return w*h/(width*height);
}

float Object::intervalOverlap(float x1, float x2, float y1, float y2)
{
	if(x2 >= y2 && y2 >= x1)
		return y2 - x1;
	if(y2 >= x2 && x2 >= y1)
		return x2 - y1;
	return 0;
}