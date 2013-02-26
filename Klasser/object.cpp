// object.ccp

#include "object.h"
#include <iostream>

using namespace std;

Object::Object(int _x, int _y, float _dx, float _dy, int _height, int _width, int _id)
:x(_x), y(_y), dx(_dx), dy(_dy), height(_height), width(_width), id(_id)  {}

void Object::info()
{
	cout << "ID: " << id << endl;
	cout << "Position: (" << x << ", " << y << ")" << endl;
	cout << "Dimenson: (" << height << ", " << width << ")" << endl;
	cout << "Velocity: (" << dx << ", " << dy << ")" << endl;
}