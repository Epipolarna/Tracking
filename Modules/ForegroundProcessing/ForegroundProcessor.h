#ifndef _FOREGROUND_PROCESSOR_H_
#define _FOREGROUND_PROCESSOR_H_

#include "../Frame.h"
#include "../FrameList.h"
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
		SHADOW = 2,
		SLOW
	};

	class ForegroundProcessor
	{
	public:
		ForegroundProcessor(){ algorithm = FAST; threshval = 50; iterations = 3; minDist = 20; };

		void segmentForeground(Frame & frame);

		// Fast algorithm (>30ms)
		// Specify threshold value and number of iterations
		void segmentForegroundFast(Frame & frame); 
		
		// Higher performance algorithm (Hopefully) (~650ms) 
		// Specify threshold value and minimum consour thickness 
		void segmentForegroundArea(Frame & frame);
		
		// Higher performance algorithm (Hopefully) (~650ms) 
		// Specify threshold value and minimum contour thickness 
		void segmentForegroundSlow(Frame & frame);

		//Manages shadows
		void segmentForegroundShadow(Frame & frame);

		void init(int iterations, double minDist, double minArea, double minQuotient, bool suppressShadows);
		void initShadow(double tau_H, double tau_S, double alpha, double beta);

		void ForegroundProcessor::setAlgortihm(Algorithm algorithm)
		{
			this->algorithm = algorithm; 
		}

	private:		
		// The shadow pixels
		Mat shadowModel;
		//Finds objects in a binary image and puts them in the list.
		void getObjects(Frame & frame);
		//Finds objects that are large and convex enough
		void getObjectsArea( Frame & frame, double maxArea, double minQuotient );
		//Manages shadows
		void suppressShadows(Frame & frame, double minArea, double minDist);
	
		//Image Processing of foreground.
		void distanceFilter(Frame & frame, double minDist);
		void threshMap(cv::Mat foreground, int threshval);
		void openingBinMap(cv::Mat foreground, int iterations = 1);
		void closingBinMap(cv::Mat foreground, int iterations = 1);
		void erodeBinMap(cv::Mat foreground, int iterations = 1);	
		void dilateBinMap(cv::Mat foreground, int iterations = 1);

		// Internal framecounter
		int frameCounter;

		// Settings
		Algorithm algorithm;
		bool shadows;
		int threshval, iterations;
		double minArea, minDist, minQuotient, tau_H, tau_S, alpha, beta;
		

	};
}


#endif