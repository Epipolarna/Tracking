#ifndef __FRAMELIST_H_
#define __FRAMELIST_H_

#include <opencv2/core/core.hpp>
#include <list>
#include <vector>
#include <ctime>
#include "Frame.h"

using namespace cv;
using namespace std;

class FrameList
{
public:
	FrameList(int framesToKeep);

	void open(std::string path);
	Frame & getLatestFrame();
	list<Frame> & getFrames();
	bool isSourceEmpty();
	void queryNextFrame();



	int getFrameAmount();
	int getFrameRate();
	int getCurrentFrameNumber();

	//Profiling
	void setTime(std::string name, double time);

	//Visualisation
	void display(std::string windowID);
	void displayProbabilityMap(std::string windowID);
	void displayForeground(std::string windowID);

	void displayInfo(std::string windowID);

	vector<Frame> toVector();

private:
	list<Frame> oldFrames;
	//CvCapture * source;
	VideoCapture source;
	int frameAmount, frameRate;
	int maxFrames;
	int currentFrameNumber;
	
	void appendFrame(IplImage *frameImage);

	//Debug
	Mat probMap;
	Mat infoDisplayMatrix;
};

#endif