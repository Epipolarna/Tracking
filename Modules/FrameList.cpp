#include "FrameList.h"

FrameList::FrameList(int framesToKeep)
{
	maxFrames = framesToKeep;
	//probMap = imread("starsCorner.tif", CV_8UC1);
	infoDisplayMatrix = Mat(480,720, CV_8UC3);
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
	oldFrames.push_front(Frame(Mat(frameImage, CV_8UC3), probMap));
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

void FrameList::setTime(std::string name, double time)
{
	getLatestFrame().profileData[name] = time;
}

void FrameList::display(std::string windowID)
{
	int fontFace = CV_FONT_HERSHEY_COMPLEX;
	double fontScale = 0.5;
	int thickness = 1;

	std::string text = "[Frame "+std::to_string(getCurrentFrameNumber())+"("+std::to_string(getFrameAmount())+")]";
	getLatestFrame().drawObjects(cv::Scalar(250, 0, 0, 255));
	putText(getLatestFrame().image, text, Point(5, 15), fontFace, fontScale, Scalar::all(0), thickness, 8);
	imshow( windowID.c_str(), getLatestFrame().image );
}

void FrameList::displayProbabilityMap(std::string windowID)
{
	imshow( windowID.c_str(), getLatestFrame().backgroundProbMap );
}

void FrameList::displayForeground(std::string windowID)
{
	imshow( windowID.c_str(), getLatestFrame().probMap );
}

#define PUTTEXT(x,y,text) putText(infoDisplayMatrix, text, Point(x, y), fontFace, fontScale, Scalar::all(0), thickness, 8);

void FrameList::displayInfo(std::string windowID)
{
	Frame * frame = &getLatestFrame();
	if(oldFrames.size() > 2)
		frame = &(*(++oldFrames.begin()));

	int fontFace = CV_FONT_HERSHEY_COMPLEX;
	double fontScale = 1;
	int thickness = 1;

	infoDisplayMatrix = Scalar::all(200);
	string text;
	int baseline;

	PUTTEXT(5,25,"Profiling:");
	int l = 60;
	for(std::map<std::string, double>::iterator i = frame->profileData.begin(); i != frame->profileData.end(); i++)
	{
		text = "   "+i->first+":";
		while(getTextSize(text, fontFace, fontScale, thickness, &baseline).width < 400)
			text += " ";
		text += std::to_string(i->second);
		PUTTEXT(5,l,text);
		l += 35;
	}

	imshow( windowID.c_str(), infoDisplayMatrix);
}