#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

#include <stdio.h>
#include "FrameList.h"

using namespace cv;
using namespace std;

int main(int argc, char** argv){

	CvCapture *movie;
	int numFrames;
	IplImage* frameImage = NULL;
	double framerate;


	FrameList frameList = FrameList(1000);

	movie = cvCaptureFromFile("../../data/camera1.mov");
	numFrames = cvGetCaptureProperty(movie,CV_CAP_PROP_FRAME_COUNT);
	framerate = cvGetCaptureProperty(movie,CV_CAP_PROP_FPS);


	//here we play the movie
	cvNamedWindow("original", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("background", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("objects", CV_WINDOW_AUTOSIZE);

	//read image frameList into frame objects
	for(int i=0; i < frameList.getMaxFrames(); i++){
		frameImage = cvQueryFrame(movie);
		frameList.appendFrame(frameImage);
	}

	cout << "read:" << frameList.getNumFrames() << "frames" << endl;
	cout << "with framerate: " << framerate << " fps" << endl;

	imshow("original", frameList.getLast().image);

	waitKey(0);

	return 0;
}