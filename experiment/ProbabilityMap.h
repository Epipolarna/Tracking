#ifndef __PROBABILITYMAP__
#define __PROBABILITYMAP__

#include <opencv2/core/core.hpp>
#include <math.h>
#include <iostream>

#include "Frame.h"

class Frame;

using namespace cv;
using namespace std;

struct gauss3D{
	uchar sigma[3];
	uchar mean[3];
	float w;
};

class ProbabilityMap{
public:
	Mat pImage;

	ProbabilityMap(Frame *prevFrame, Frame *currFrame);
	
private:
	int numGauss; //how many distributions will be used
	float lambda; //treshold for belonging to a distribution
	float initSigma; //inital sigma for new distribution
	
	gauss3D *distributions;
	
	void updateDistributions(Frame *prevFrame,Frame *currFrame);
};

#endif