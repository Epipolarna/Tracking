#ifndef __PROBABILITYMAP__
#define __PROBABILITYMAP__

#include <opencv2/core/core.hpp>
#include <math.h>
#include <iostream>

#include "PixelModel.h"

class Frame;

using namespace cv;
using namespace std;

class ProbabilityMap{
public:
	Mat pImage;
	Mat estimatedBackground;
	Mat backgroundCertainty;
	
	ProbabilityMap() {};
	ProbabilityMap(Frame *currFrame);
	ProbabilityMap(Frame *prevFrame, Frame *currFrame);

	//private bits
	PixelModel *models;
};

#include "../Frame.h"

#endif