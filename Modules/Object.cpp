// object.ccp

#include "Object.h"
#include <iostream>

using namespace std;

Object::Object(int x, int y, float dx, float dy, float xHat, float yHat, int width, int height, int id)
: x(x), y(y), dx(dx), dy(dy), xHat(xHat), yHat(yHat), width(width), height(height), id(id)  
{
	boundingBox.x = x-width/2;
	boundingBox.y = y-height/2;
	boundingBox.width = width;
	boundingBox.height = height;
	lost = false;
	xHat = (float)x;
	yHat = (float)y;
}

Object::Object(cv::Rect boundingBox, float dx, float dy, float xHat, float yHat, int id)
: boundingBox(boundingBox), dx(dx), dy(dy), xHat(xHat), yHat(yHat), id(id)
{
	x = boundingBox.x + boundingBox.width/2;
	y = boundingBox.y + boundingBox.height/2;
	xHat = (float)x;
	yHat = (float)y;
	width = boundingBox.width;
	height = boundingBox.height;
}

std::ostream & operator<< (std::ostream & o, Object & object)
{
	o << "ID: " << object.id << "\n";
	o << "Position: (" << object.x << ", " << object.y << ")\n";
	o << "PEstimate: (" << object.xHat << ", " << object.yHat << ")\n";
	o << "Dimenson: (" << object.width << ", " << object.height << ")\n";
	o << "Velocity: (" << object.dx << ", " << object.dy << ")\n";
	o << "Lost: " << object.lost << "\n";
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
	float w = intervalOverlap((float)x-width/2, (float)(x+width/2), (float)other.x-other.width/2, (float)other.x+other.width/2);
	float h = intervalOverlap((float)y-height/2, (float)(y+height/2), (float)other.y-other.height/2, (float)other.y+other.height/2);
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