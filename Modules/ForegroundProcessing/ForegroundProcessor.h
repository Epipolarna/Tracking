#ifndef _FOREGROUND_PROCESSOR_H_
#define _FOREGROUND_PROCESSOR_H_

#include "../Frame.h"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

///////////////// Module ////////////////
/////////////////////////////////////////
namespace ForegroundProcessing
{
	enum Algorithm
	{
		FAST = 0,
		AREA = 1,
		SLOW
	};

	class ForegroundProcessor
	{
	public:
		ForegroundProcessor(){ algorithm = FAST; threshval = 50; iterations = 3; minDist = 20; };

		void segmentForeground(Frame & frame);

		// Fast algorithm (>30ms)
		// Specify threshold value and number of iterations
		void segmentForegroundFast(Frame & frame, int threshval, int iterations); 
		
		// Higher performance algorithm (Hopefully) (~650ms) 
		// Specify threshold value and minimum consour thickness 
		void segmentForegroundArea(Frame & frame, int threshval, int iterations, double minArea, double minQuotient);
		
		// Higher performance algorithm (Hopefully) (~650ms) 
		// Specify threshold value and minimum consour thickness 
		void segmentForegroundSlow(Frame & frame, int threshval, double minDist);

		void init(int threshval, int Iterations, double minDist, double minArea, double minQuotient);
		
		void ForegroundProcessor::setAlgortihm(Algorithm algorithm)
		{
			this->algorithm = algorithm; 
		}

	private:
		//Finds objects in a binary image and puts them in the list.
		void getObjects(Frame & frame);

		void getObjectsArea( Frame & frame, double maxArea, double minQuotient );
		//Same as above but also performs a cleanup using the distance transform.
		void getObjectsDistMap(Frame & frame, double minDist);
	
		//Image Processing of probabilitymap.
		void threshMap(cv::Mat probMap, int threshval);
		void openingBinMap(cv::Mat probMap, int iterations = 1);
		void closingBinMap(cv::Mat probMap, int iterations = 1);
		void erodeBinMap(cv::Mat probMap, int iterations = 1);	
		void dilateBinMap(cv::Mat probMap, int iterations = 1);

		// Settings
		Algorithm algorithm;
		int threshval, iterations;
		double minArea, minDist, minQuotient;

	};
}


#endif