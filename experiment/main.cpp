#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

#include "FrameList.h"

using namespace cv;
using namespace std;

int main(int argc, char** argv){

	CvCapture *movie;
	int numFrames;
	IplImage* frameImage = NULL;
	int frameNumber = 0;

	FrameList frameList;

	movie = cvCaptureFromFile("../../data/camera1.mov");
	numFrames = cvGetCaptureProperty(movie,CV_CAP_PROP_FRAME_COUNT);


	//read image frameList into frame objects
	for(int i=0; i < numFrames; i++){
		frameImage = cvQueryFrame(movie);
		frameList.appendFrame(frameImage);
	}

	cout << "read:" << frameList.getNumFrames() << "frames" << endl;

	return 0;
}