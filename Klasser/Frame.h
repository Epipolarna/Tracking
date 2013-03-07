#ifndef __FRAME__
#define __FRAME__

#include <opencv2/core/core.hpp>
#include <vector>
#include "object.h"

#include "ProbabilityMap.h"

using namespace cv;

class Frame{
public:
	Frame(Mat img);

public:
	Mat image;
	ProbabilityMap *p;
	
	std::list<Object> objects;
};

#endif