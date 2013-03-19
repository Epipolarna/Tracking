#ifndef _FOREGROUND_PROCESSOR_H_
#define _FOREGROUND_PROCESSOR_H_

#include "frame.h"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

///////////////// Module ////////////////
/////////////////////////////////////////
namespace ForegroundProcessing
{
	class ForegroundProcessor
	{
	public:
		// Fast algorithm (>30ms)
		// Specify threshold value and number of iterations
		void segmentForegroundFast(Frame & frame, int threshval, int iterations); 
		
		// Higher performance algorithm (Hopefully) (~650ms) 
		// Specify threshold value and minimum consour thickness 
		void segmentForegroundSlow(Frame & frame, int threshval, double minDist); 

	private:
		//Finds objects in a binary image and puts them in the list.
		void getObjects(Frame & frame);
		//Same as above but also performs a cleanup using the distance transform.
		void getObjectsDistMap(Frame & frame, double minDist);
	
		//Image Processing of probabilitymap.
		void threshMap(cv::Mat probMap, int threshval);
		void openingBinMap(cv::Mat probMap, int iterations = 1);
		void closingBinMap(cv::Mat probMap, int iterations = 1);
		void erodeBinMap(cv::Mat probMap, int iterations = 1);	
		void dilateBinMap(cv::Mat probMap, int iterations = 1);

	};
}


#endif