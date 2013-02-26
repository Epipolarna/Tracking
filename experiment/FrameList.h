#ifndef __FRAMELIST__
#define __FRAMELIST__

#include <opencv2/core/core.hpp>
#include <vector>

#include "Frame.h"

using namespace cv;
using namespace std;

class FrameList{
public:
	vector<Frame> framesList;
	int foo();
	void appendFrame(IplImage *frameImage);
	int getNumFrames();
};

#endif