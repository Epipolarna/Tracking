// ForecroundProcessor.cpp

#include "ForegroundProcessor.h"
//#include "stdafx.h"

namespace ForegroundProcessing
{

	////////////////// Foreground Segmentation //////////////////////
	void ForegroundProcessor::segmentForeground(Frame & frame)
	{
		switch(algorithm)
		{
		case 0:
			segmentForegroundFast(frame, threshval, iterations);
			break;
		case 1:
			segmentForegroundArea(frame, threshval, iterations, minArea, minQuotient);
			break;
		case 2:
			segmentForegroundSlow(frame, threshval, minDist);
			break;
		}
	}

	void ForegroundProcessor::segmentForegroundFast(Frame & frame, int threshval, int iterations)
	{
		threshMap(frame.probMap, threshval); //Threshold at threshval
	
		// Erode followed by 3 iterations of dilate (3x3 kernel)
		openingBinMap(frame.probMap, iterations); 
	
		getObjects(frame);
		return;
	}

	void ForegroundProcessor::segmentForegroundSlow(Frame & frame, int threshval, double minDist)
	{
		threshMap(frame.probMap, threshval); //Threshold at threshval
	
		getObjectsDistMap(frame, minDist);
	
		return;
	}

	void ForegroundProcessor::segmentForegroundArea(Frame & frame, int threshval, int Iterations, double minArea, double minQuotient)
	{
		//threshMap(frame.probMap, threshval); //Threshold at threshval
	
		closingBinMap(frame.probMap, iterations); 

		getObjectsArea(frame, minArea, minQuotient);
	
		return;
	}


	
	void ForegroundProcessor::init(int threshval, int iterations, double minDist, double minArea, double minQuotient)
	{
		this->algorithm = algorithm;
		this->threshval = threshval;
		this->iterations = iterations;
		this->minDist = minDist;
		this->minArea = minArea; 
		this->minQuotient =	minQuotient;
	}

	////////////////// Private Functions //////////////////////
	///////////////////////////////////////////////////////////
	
	////////////////// Object Detection ///////////////////////
	void ForegroundProcessor::getObjects(Frame & frame)
	{
		vector<vector<Point>> contours;
		findContours( frame.probMap.clone(), contours, CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	
		for(unsigned int i = 0; i < contours.size(); i++)
		{		
			//Create an object for every countour using the boundingRect command
			frame.objects.push_back(Object(boundingRect(contours[i])));	
		}
	}

		void ForegroundProcessor::getObjectsArea(Frame & frame, double minArea, double minQuotient)
	{
		double objArea;
		Rect objRect;
		vector<vector<Point>> contours;
		findContours( frame.probMap.clone(), contours, CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	
		for(unsigned int i = 0; i < contours.size(); i++)
		{		
			objArea = contourArea(contours[i]);
			objRect = boundingRect(contours[i]);
			
			if ((objArea > minArea) && (objArea > (objRect.width * objRect.height)/minQuotient))
			{
				//Create an object for every countour using the boundingRect command
				frame.objects.push_back(Object(objRect));	
			}
			
			//
			//frame.objects.push_back(Object(boundingRect(contours[i])));	
		}
	}

	void ForegroundProcessor::getObjectsDistMap(Frame & frame, double minDist)
	{
		vector<vector<Point>> contours;
		findContours( frame.probMap.clone(), contours, CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

		Rect objRect;
		double dist = 0;
		double minSize = 20;
		for(unsigned int i = 0; i < contours.size(); i++)
		{	
			objRect = boundingRect(contours[i]);
			vector<Point> contour = contours[i];
		
			//Measure distance to the contour of all pixels within the bounding box.
			for( int j = objRect.x; j < objRect.x + objRect.width; j++)
				{ for( int k = objRect.y; k < objRect.y + objRect.height; k++) 
					{ 
						if (pointPolygonTest(contour, Point(j, k), false) == 1) //If object is inside the contour
						{
							dist = max(dist, pointPolygonTest(contour, Point(j, k), true)); // Calculate distance
						} 
					}		
				}
			if (dist > minDist) //Create object only if distance is great enough.
			{		
				frame.objects.push_back(Object(objRect));	
			}
			dist = 0;
		}
	}

	////////////////// Image Processing //////////////////////
	void ForegroundProcessor::threshMap(Mat probMap, int threshval)
	{
		int maxVal = 255;
		threshold(probMap, probMap, threshval, maxVal, THRESH_BINARY);
	}

	void ForegroundProcessor::openingBinMap(Mat probMap, int iterations)
	{
		cv::Mat kernel;
		kernel = getStructuringElement( MORPH_CROSS, Size(3, 3));
		erode(probMap, probMap, kernel, cv::Point(-1,-1), iterations);
		dilate(probMap, probMap, kernel, cv::Point(-1,-1), iterations);
	}

	void ForegroundProcessor::closingBinMap(Mat probMap, int iterations)
	{
		cv::Mat kernel;
		kernel = getStructuringElement( MORPH_CROSS, Size(3, 3));
		dilate(probMap, probMap, kernel, cv::Point(-1,-1), iterations);
		erode(probMap, probMap, kernel, cv::Point(-1,-1), iterations);
	}

	void ForegroundProcessor::erodeBinMap(Mat probMap, int iterations)
	{
		cv::Mat kernel;
		kernel = getStructuringElement( MORPH_RECT, Size(3, 3));
		erode(probMap, probMap, kernel, cv::Point(-1,-1), iterations);
	}

	void ForegroundProcessor::dilateBinMap(Mat probMap, int iterations)
	{
		cv::Mat kernel;
		kernel = getStructuringElement( MORPH_RECT, Size(3, 3));
		dilate(probMap, probMap, kernel, cv::Point(-1,-1), iterations);
	}

}
