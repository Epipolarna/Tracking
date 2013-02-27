#ifndef __FRAME__
#define __FRAME__

#include <opencv2/core/core.hpp>
#include <vector>

#include "ProbabilityMap.h"

using namespace cv;

class Frame{
public:
	Mat image;
	ProbabilityMap *p;

	Frame(Mat img);
};

#endif