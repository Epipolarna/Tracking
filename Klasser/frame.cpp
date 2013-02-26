/* 
*	frame.cpp
*	Author: GAMMa
*
*/


#include "stdafx.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

//#include "frame.h"

using namespace cv;
using namespace std;

frame::frame(cv::Mat _rawFrame, cv::Mat _probMap)
	:rawFrame(_rawFrame), probMap(_probMap) {}

void frame::threshMap(int threshval)
{
	int maxVal = 255;
	threshold(probMap, probMap, threshval, maxVal, THRESH_BINARY);
}

void frame::openingBinMap(int iterations)
{
	cv::Mat kernel;
	kernel = getStructuringElement( MORPH_ELLIPSE, Size(3, 3));			     //Kernel creation
	dilate(probMap, probMap, kernel, cv::Point(-1,-1), iterations);			
	erode(probMap, probMap, kernel, cv::Point(-1,-1), iterations);
}

void frame::closingBinMap(int iterations)
{
	cv::Mat kernel;
	kernel = getStructuringElement( MORPH_ELLIPSE, Size(3, 3));
	dilate(probMap, probMap, kernel, cv::Point(-1,-1), iterations);
	erode(probMap, probMap, kernel, cv::Point(-1,-1), iterations);
}


/////////////////////////////////////////////// Private Functions //////////////////////////////////////////////////////

void frame::erodeBinMap(int iterations)
{
	cv::Mat kernel;
	kernel = getStructuringElement( MORPH_ELLIPSE, Size(3, 3));
	erode(probMap, probMap, kernel, cv::Point(-1,-1), iterations);
}

void frame::dilateBinMap(int iterations)
{
	cv::Mat kernel;
	kernel = getStructuringElement( MORPH_ELLIPSE, Size(3, 3));
	dilate(probMap, probMap, kernel, cv::Point(-1,-1), iterations);
}





