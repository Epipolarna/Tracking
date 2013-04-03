/* TSBB15 Tracking project 2013
 * - - - - - - - - - - - - - - -
 * Principal contributer: Martin Svensson & Mattias Tiger
 */

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
	
	bool queryNextFrame();
	
	int getFrameAmount();
	
	int getFrameRate();
	int getCurrentFrameNumber();

	Size movieSize;

	//Profiling
	void setTime(std::string name, double time);

	//Visualisation
	void display(std::string windowID);
	void displayBackground(std::string windowID);
	void displayBackgroundEstimate(std::string windowID);
	void displayBackgroundCertainty(std::string windowID);
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
	bool eofReached;
	
	void appendFrame(IplImage *frameImage);

	//Debug
	Mat infoDisplayMatrix;
};

#endif