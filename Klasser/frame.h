/* 
*	frame.h
*	Author: GAMMa
*
*/


#ifndef FRAME_H
#define FRAME_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

class frame
{

public:
	frame(cv::Mat rawFrame, cv::Mat probMap);
	
	//Image Processing of binary image
	void threshMap(int threshval);
	void openingBinMap(int iterations = 1);
	void closingBinMap(int iterations = 1);
	void labelingBinMap();
	//void labeling();
	
	cv::Mat probMap;
	cv::Mat rawFrame;
	
	//For image region labeling (not implemented yet)
	vector<vector<Point>> contours;


protected:

private:
	//Might need these later
	void erodeBinMap(int iterations = 1);	
	void dilateBinMap(int iterations = 1);

};


#endif
