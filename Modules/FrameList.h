#ifndef __FRAMELIST__
#define __FRAMELIST__

#include <opencv2/core/core.hpp>
#include <list>
#include <vector>
#include "Frame.h"

using namespace cv;
using namespace std;

class FrameList
{
public:
	FrameList::FrameList(int framesToKeep);

	void open(std::string path);
	Frame & getLatestFrame();
	list<Frame> & getFrames();
	bool isSourceEmpty();
	void queryNextFrame();

	int getFrameAmount();
	int getFrameRate();
	int getCurrentFrameNumber();

	//Visualisation
	void display(std::string windowID);
	void displayBackground(std::string windowID);
	void displayForeground(std::string windowID);

	vector<Frame> toVector();

private:
	list<Frame> oldFrames;
	CvCapture * source;
	int frameAmount, frameRate;
	int maxFrames;
	int currentFrameNumber;
	
	void FrameList::appendFrame(IplImage *frameImage);
};

#endif