// object.h, Contains class definition of object

#include <iostream>

#ifndef OBJECT_H
#define OBJECT_H

class Object
{
public:
	Object() {x = 0, y = 0, dx = 0, dy = 0, width = 0, height = 0, id = 0;}
	Object(int x, int y, float dx, float dy, int width = 0, int height = 0, int id = 0);

	int id, x, y, width, height;
	float dx, dy;
		
	friend std::ostream & operator<< (std::ostream & o, Object & obj);
	void info();
};
#endif