// object.ccp

#include "stdafx.h"

//#include "object.h"
#include <iostream>

using namespace std;

Object::Object(int x, int y, float dx, float dy, int width, int height, int id)
: x(x), y(y), dx(dx), dy(dy), width(width), height(height), id(id)  
{
	boundingBox.x = x-width/2;
	boundingBox.y = y-height/2;
	boundingBox.width = width;
	boundingBox.height = height;
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
	o << "Velocity: (" << object.dx << ", " << object.dy << ")" << "\n";
	return o;
}

void Object::info()
{
	cout << "ID: " << id << endl;
	cout << "Position: (" << x << ", " << y << ")" << endl;
	cout << "Dimenson: (" << width << ", " << height << ")" << endl;
	cout << "Velocity: (" << dx << ", " << dy << ")" << endl;
}