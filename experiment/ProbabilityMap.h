#ifndef __PROBABILITYMAP__
#define __PROBABILITYMAP__

#include <opencv2/core/core.hpp>

#include "Frame.h"

class Frame;

using namespace cv;

class ProbabilityMap{
public:
	Mat pImage;

	ProbabilityMap(Frame *prevFrame, Frame *currFrame);
	
private:
	int k; //how many distributions will be used
	Mat sigma[3];
	Mat mean[3];
	void updateDistributions(Frame *currFrame);
};

#endif