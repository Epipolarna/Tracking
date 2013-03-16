/* 
*	frame.h
*
*
*/


#ifndef FRAME_H
#define FRAME_H

#include "object.h"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <string>

using namespace cv;

class frame
{

public:
	frame(Mat rawFrame, Mat probMap);
	
	//Draws the bounding boxes of the objects in object list.
	void drawBoundingBox(cv::Scalar color);
	void getObjects();

	//Image Processing of probabilitymap.
	void threshMap(int threshval);
	void openingBinMap(int iterations = 1);
	void closingBinMap(int iterations = 1);
	
	//Data
	Mat rawFrame;		//Should be 3-channel color
	Mat probMap;		//Should be single channel grayscale
	std::list<Object> objects;
	
	//For debugging/showoff
	void showObjects();
	void showImageRaw(std::string windowID);
	void showImageProbMap(std::string windowID);
	
protected:

private:
	//Might need these later
	void erodeBinMap(int iterations = 1);	
	void dilateBinMap(int iterations = 1);

};


#endif
