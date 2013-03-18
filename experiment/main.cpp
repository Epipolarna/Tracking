#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

#include <stdio.h>
#include "FrameList.h"
#include "Frame.h"
#include "ProbabilityMap.h"

using namespace cv;
using namespace std;

int main(int argc, char** argv){

	CvCapture *movie;
	int numFrames;
	IplImage* frameImage = NULL;
	double framerate;


	FrameList frameList = FrameList(100);

	movie = cvCaptureFromFile("../../data/camera1.mov");
	numFrames = cvGetCaptureProperty(movie,CV_CAP_PROP_FRAME_COUNT);
	framerate = cvGetCaptureProperty(movie,CV_CAP_PROP_FPS);


	//read image frameList into frame objects
	for(int i=0; i < frameList.getMaxFrames(); i++){
		frameImage = cvQueryFrame(movie);
		frameList.appendFrame(frameImage);
	}

	cout << "read:" << frameList.getNumFrames() << "frames" << endl;
	cout << "with framerate: " << framerate << " fps" << endl;

	//grab one more frame
	frameImage = cvQueryFrame(movie);

	for(int i=0; i < frameList.getMaxFrames(); i++){
		frameImage = cvQueryFrame(movie);
		frameList.appendFrame(frameImage);
	}

	cout << "generating probability map" << endl;
	//generate a probability map
	Frame* f = new Frame(frameImage);
	Frame lastFrame = frameList.getLast();

	ProbabilityMap p = ProbabilityMap(f,&lastFrame);

	cout << "displaying image" << endl;
	namedWindow("probability",CV_WINDOW_AUTOSIZE);
	imshow("probability",p.pImage);

	waitKey(0);

	return 0;
}