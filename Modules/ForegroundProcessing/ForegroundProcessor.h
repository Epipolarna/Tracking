/* TSBB15 Tracking project 2013
 * - - - - - - - - - - - - - - -
 * Principal contributer: Martin Svensson
 */

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

		// Fast, single opening/closing + detection
		void segmentForegroundFast(Frame & frame); 
		
		// Discard too small objects. 
		void segmentForegroundArea(Frame & frame);

		//Manages shadows, if shaduwsuppression is true, otherwise, same as area
		void segmentForegroundShadow(Frame & frame);

		// Uses distance transform for garbage cleanup, with shadow suppression 
		void segmentForegroundSlow(Frame & frame);

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