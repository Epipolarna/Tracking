#ifndef __FRAME__
#define __FRAME__

#include <opencv2/core/core.hpp>
#include "ProbabilityMap.h"

class ProbabilityMap;

using namespace cv;

class Frame{
public:
	Mat image;
	ProbabilityMap *pData;

	Frame(Mat img);
};

#endif