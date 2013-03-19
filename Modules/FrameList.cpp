#include "FrameList.h"

FrameList::FrameList(int framesToKeep)
{
	maxFrames = framesToKeep;
}

void FrameList::open(std::string path)
{
	source = cvCaptureFromFile(path.c_str());
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
	return getCurrentFrameNumber() >= frameAmount;
}

void FrameList::queryNextFrame()
{
	if(getFrameAmount() <= getCurrentFrameNumber())
		return;

	appendFrame(cvQueryFrame(source));
	currentFrameNumber++;
}

void FrameList::appendFrame(IplImage *frameImage)
{
	if(getCurrentFrameNumber() >= maxFrames)
	{
		oldFrames.pop_back();
	}
	oldFrames.push_front(Frame(Mat(frameImage), Mat(frameImage)));
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

void FrameList::display(std::string windowID)
{
	int fontFace = CV_FONT_HERSHEY_COMPLEX;
	double fontScale = 0.5;
	int thickness = 1;

	std::string text = "[Frame "+std::to_string(getCurrentFrameNumber())+"("+std::to_string(getFrameAmount())+")]";
	putText(getFrames().front().rawFrame, text, Point(5, 15), fontFace, fontScale, Scalar::all(0), thickness, 8);
	imshow( windowID.c_str(), getFrames().front().rawFrame );
}

void FrameList::displayBackground(std::string windowID)
{
	//<TODO>
}

void FrameList::displayForeground(std::string windowID)
{
	//<TODO>
}
