// object.ccp

#include "object.h"
#include <iostream>

using namespace std;

Object::Object(int x, int y, float dx, float dy, int width, int height, int id)
: x(x), y(y), dx(dx), dy(dy), width(width), height(height), id(id)  {}

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