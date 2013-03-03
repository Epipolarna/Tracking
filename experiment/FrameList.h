#ifndef __FRAMELIST__
#define __FRAMELIST__

#include <opencv2/core/core.hpp>
#include <list>
#include <vector>

#include "Frame.h"

using namespace cv;
using namespace std;

class FrameList{
public:
	list<Frame> oldFrames;
	
	//public methods
	FrameList(int maxFrames);
	void appendFrame(IplImage *frameImage);
	int getNumFrames();
	int getMaxFrames();
	Frame getLast();
	vector<Frame> toVector();

private:
	int maxFrames;
};

#endif