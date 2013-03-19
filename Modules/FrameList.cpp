#include "FrameList.h"

FrameList::FrameList(int framesToKeep)
{
	maxFrames = framesToKeep;
}

void FrameList::open(std::string path)
{
	source = cvCaptureFromFile(path);
	frameAmount = cvGetCaptureProperty(source,CV_CAP_PROP_FRAME_COUNT);
	frameRate = cvGetCaptureProperty(source,CV_CAP_PROP_FPS);
	currentFrameNumber = 0;

	// Load the first frame from source
	queryNextFrame();
}

Frame & FrameList::getLatestFrame()
{
	return oldFrames.front();
}

list<Frame> & FrameList::getFrames()
{
	return oldFrames;
}

bool FrameList::isSourceEmpty()
{
	return currentFrame >= frameAmount;
}

void FrameList::queryNextFrame()
{
	if(getFrameAmount() <= getCurrentFrameNumber())
		return;

	appendFrame(cvQueryFrame(source));
	currentFrame++;
}

void FrameList::appendFrame(IplImage *frameImage)
{

	if(getFrameAmount() >= maxFrames)
	{
		oldFrames.pop_front();
	}
	oldFrames.push_back(Frame(Mat(frameImage), Mat(frameImage)));
}

int FrameList::getFrameAmount()
{
	return frameAmount;
}

int FrameList::getCurrentFrameNumber()
{
	return currentFrameNumber;
}

int FrameList::getFrameRate()
{
	return frameRate;
}

vector<Frame> FrameList::toVector()
{
	return vector<Frame>(oldFrames.begin(),oldFrames.end());
}

void display(std::string windowID)
{
	//<TODO>
}

void displayBackground(std::string windowID)
{
	//<TODO>
}

void displayForeground(std::string windowID)
{
	//<TODO>
}
