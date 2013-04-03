/* TSBB15 Tracking project 2013
 * - - - - - - - - - - - - - - -
 * Principal contributer: Martin Svensson & Mattias Tiger
 */

#ifndef _FRAME_H_
#define _FRAME_H_

#include "Object.h"
#include "BackgroundModelling/ProbabilityMap.h"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <string>
#include <map>

using namespace cv;

class ProbabilityMap;

class Frame
{
public:
	Frame() {}	//Default constructor, used by Tiger for testing..
	Frame(Mat image);
	
	//Draws the bounding boxes and velocity vectors of the objects in object list.
	void drawObjects(cv::Scalar color);
	void drawObjects(std::vector<Object> & objects, Scalar color);
	void drawObjectsPrediction(Scalar color);
		
	//Data
	Mat demoImage;
	Mat image;						 //Should be 3-channel color
	Mat backgroundProbMap;			 //Should be 3-channel color
	//Mat probMap;					 //Should be single channel grayscale
	Mat foreground;					 //Should be single uchar (CV_8UC1)
	std::vector<Object> objects;
	std::map<std::string, double> profileData;	// Time in seconds
	ProbabilityMap bgModel;

	bool displayUncertainty;

	//For debugging/showoff
	void init(bool displayUncertainty);
	void showObjects();
	void showImageRaw(std::string windowID);
	void showImageProbMap(std::string windowID);
};


#endif
