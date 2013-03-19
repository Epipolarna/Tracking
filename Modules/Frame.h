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

//using namespace std;
using namespace cv;

class Frame
{

public:
	Frame() {}	//Default constructor, used by Tiger for testing..
	Frame(Mat rawFrame, Mat probMap);
	
	//Draws the bounding boxes and velocity vectors of the objects in object list.
	void drawObjects(cv::Scalar color);
	void drawObjects(std::vector<Object> & objects, Scalar color);
	
	//Finds objects in a binary image and puts them in the list.
	void getObjects();
	//Same as above but also performs a cleanup using the distance transform. (slow but hopefully better)
	void getObjectsDistMap(double minDist);
	

	//Image Processing of probabilitymap.
	void threshMap(int threshval);
	void openingBinMap(int iterations = 1);
	void closingBinMap(int iterations = 1);
	
	//Data
	Mat rawFrame;		//Should be 3-channel color
	Mat probMap;		//Should be single channel grayscale
	std::vector<Object> objects;
	
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
