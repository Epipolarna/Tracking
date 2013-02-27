#ifndef __PROBABILITYMAP__
#define __PROBABILITYMAP__

#include <opencv2/core/core.hpp>
#include <vector>

using namespace cv;

class Frame;

class ProbabilityMap{
public:
	Mat bgProbability;

	ProbabilityMap(vector<Frame> *prevFrames, Mat *currFrameImage);

	Mat getBackground(Frame *prevFrame, Mat *currFrameImage);
};

#endif