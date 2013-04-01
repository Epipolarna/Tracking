// ForecroundProcessor.cpp

#include "ForegroundProcessor.h"
//#include "stdafx.h"

namespace ForegroundProcessing
{

	////////////////// Foreground Segmentation //////////////////////
	void ForegroundProcessor::segmentForeground(Frame & frame)
	{
		frame.foreground = frame.bgModel.pImage.clone();
		switch(algorithm)
		{
		case 0:
			segmentForegroundFast(frame);
			break;
		case 1:
			segmentForegroundArea(frame);
			break;
		case 2:
			segmentForegroundShadow(frame);
		case 3:
			segmentForegroundSlow(frame);
			break;
		}
	}

	void ForegroundProcessor::init(int iterations, double minDist, double minArea, double minQuotient)
	{
		this->threshval = 192;
		this->iterations = iterations;
		this->minDist = minDist;
		this->minArea = minArea; 
		this->minQuotient =	minQuotient;
		this->frameCounter = 0;
	}

	void ForegroundProcessor::segmentForegroundFast(Frame & frame)
	{
		threshMap(frame.foreground, threshval); //Threshold at threshval
	
		// Erode followed by 3 iterations of dilate (3x3 kernel)
		openingBinMap(frame.foreground, iterations); 
	
		getObjects(frame);
		return;
	}

	void ForegroundProcessor::segmentForegroundSlow(Frame & frame)
	{
		threshMap(frame.foreground, threshval); //Threshold at threshval
		suppressShadows(frame, minArea, minQuotient);
		getObjectsDistMap(frame, minDist);
	
		return;
	}

	void ForegroundProcessor::segmentForegroundArea(Frame & frame)
	{
		threshMap(frame.foreground, threshval); //Threshold to create binary image.
		openingBinMap(frame.foreground, iterations);
		dilateBinMap(frame.foreground, iterations);
		closingBinMap(frame.foreground, iterations);
		//openingBinMap(frame.foreground, 2);
		//closingBinMap(frame.foreground, iterations); 

		getObjectsArea(frame, minArea, minQuotient);
	
		return;
	}
	

	void ForegroundProcessor::segmentForegroundShadow(Frame & frame)
	{
		
		threshMap(frame.foreground, threshval);
		suppressShadows(frame, minArea, minQuotient);
		
		//Debug
		Mat debug = frame.foreground;
		imshow( "Shadow Debug", debug );
		
		//Remove the gray debug pixels
		threshMap(frame.foreground, 170);
		
		openingBinMap(frame.foreground, iterations);
		dilateBinMap(frame.foreground, iterations);
		closingBinMap(frame.foreground, iterations);

		getObjectsArea(frame, minArea, minQuotient);
		
		//Debug (removes everything the getObjectsArea method ignores)
		vector<vector<Point>> contours;
		findContours( frame.foreground.clone(), contours, CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
		double objArea;
		Rect objRect;
		
		for(unsigned int i = 0; i < contours.size(); i++)
		{		
			objArea = contourArea(contours[i]);
			if (objArea < minArea && (objArea > (objRect.width * objRect.height)/minQuotient))
			{
				drawContours(frame.foreground, contours, i, Scalar(0), CV_FILLED); 
			}
		}	
		
		return;
	}


	////////////////// Private Functions //////////////////////
	///////////////////////////////////////////////////////////
	
	////////////////// Object Detection ///////////////////////
	void ForegroundProcessor::getObjects(Frame & frame)
	{
		vector<vector<Point>> contours;
		findContours( frame.foreground.clone(), contours, CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	
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
		findContours( frame.foreground.clone(), contours, CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	
		for(unsigned int i = 0; i < contours.size(); i++)
		{		
			objArea = contourArea(contours[i]);
			objRect = boundingRect(contours[i]);
			
			if ((objArea > minArea) && (objArea > (objRect.width * objRect.height)/minQuotient))
			{
				//Create an object for every countour using the boundingRect command
				frame.objects.push_back(Object(objRect));	
			}	
		}
	}

	void ForegroundProcessor::suppressShadows(Frame & frame, double minArea, double minDist)
	{
		if (shadowModel.empty())
			shadowModel =  Mat::zeros(frame.image.size(), CV_8UC3);
		frameCounter++;
		if (frameCounter < 10)
		{
			shadowModel += (frame.image / 10);
			return;
		}
		int i = 7;
		if (frameCounter == 10)
		{
			//Show "most probable background"
			//imshow( "ShadowModel", shadowModel );
			//cvtColor(shadowModel, shadowModel, CV_BGR2HSV_FULL);
			
			// Debug
			/*
			Mat hue(shadowModel.size(), CV_8UC1);
			Mat saturation(shadowModel.size(), CV_8UC1);
			Mat value(shadowModel.size(), CV_8UC1);
			Mat derp[] = {hue, saturation, value};
			int from_to[] = { 0,0, 1,1, 2,2 };
			mixChannels(&shadowModel, 1, derp, 3, from_to, 3);
			imshow( "ShadowModel, hue", hue );
			imshow( "ShadowModel, saturation", saturation );
			imshow( "ShadowModel, value", value);
			*/
		}

		vector<vector<Point>> contours;
		findContours( frame.foreground.clone(), contours, CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

		Vec3f newColorVec, oldColorVec, tempVec;
		Mat lastImage = frame.image.clone();
		cvtColor(lastImage, lastImage, CV_BGR2HSV_FULL);
		
		double objArea;
		Rect objRect;
		double dist = 0;
		
		for(unsigned int i = 0; i < contours.size(); i++)
		{	
			objArea = contourArea(contours[i]);
			objRect = boundingRect(contours[i]);
			vector<Point> contour = contours[i];

			for( int j = objRect.x; j < objRect.x + objRect.width; j++)
			{ 
				for( int k = objRect.y; k < objRect.y + objRect.height; k++) 
				{ 
					Point matPos(j,k);
					//If object is not outside the contour
					if (pointPolygonTest(contour, matPos, false) >= 0) 
					{
						// Parameters for detecting cars in the RenovA clips
						/*if ( (((abs((double)lastImage.at<Vec3b>(matPos)[0] - (double)shadowModel.at<Vec3b>(matPos)[0])/255 < 0.2) ) || ( (double)shadowModel.at<Vec3b>(matPos)[1]/255 < 0.05) )
							&& ( ((double)lastImage.at<Vec3b>(matPos)[1] - (double)shadowModel.at<Vec3b>(matPos)[1])/255 < 0.3)	
							&& ( ((double)lastImage.at<Vec3b>(matPos)[2] / ((double)shadowModel.at<Vec3b>(matPos)[2])  + 0.0001) > 0.15)
							&& ( ((double)lastImage.at<Vec3b>(matPos)[2] / ((double)shadowModel.at<Vec3b>(matPos)[2]) + 0.0001) < 1.1)
							)*/
						// Parameters for the shopping mall (uber shitty)
						if ( (((abs((double)lastImage.at<Vec3b>(matPos)[0] - (double)shadowModel.at<Vec3b>(matPos)[0])/255 < 0.1) ) )//|| ( (double)shadowModel.at<Vec3b>(matPos)[1]/255 < 0.1) ) //HUE
							&& ( ((double)lastImage.at<Vec3b>(matPos)[1] - (double)shadowModel.at<Vec3b>(matPos)[1])/255 < 0.3)	 // SATURATION
							&& ( ((double)lastImage.at<Vec3b>(matPos)[2] / (double)shadowModel.at<Vec3b>(matPos)[2]) > 0.2)	//VALUE (ALPHA)
							&& ( ((double)lastImage.at<Vec3b>(matPos)[2] / (double)shadowModel.at<Vec3b>(matPos)[2]) < 0.95) //VALUE (BETA)
							)
						{
							frame.foreground.at<uchar>(matPos) = 128;
						}
					} 
				}		
			}
		}
	}
	
	
	
	void ForegroundProcessor::getObjectsDistMap(Frame & frame, double minDist)
	{
		vector<vector<Point>> contours;
		findContours( frame.foreground.clone(), contours, CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

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
	void ForegroundProcessor::threshMap(Mat foreground, int threshval)
	{
		int maxVal = 255;
		threshold(foreground, foreground, threshval, maxVal, THRESH_BINARY);
	}

	void ForegroundProcessor::openingBinMap(Mat foreground, int iterations)
	{
		cv::Mat kernel;
		kernel = getStructuringElement( MORPH_CROSS, Size(3, 3));
		erode(foreground, foreground, kernel, cv::Point(-1,-1), iterations);
		dilate(foreground, foreground, kernel, cv::Point(-1,-1), iterations);
	}

	void ForegroundProcessor::closingBinMap(Mat foreground, int iterations)
	{
		cv::Mat kernel;
		kernel = getStructuringElement( MORPH_CROSS, Size(3, 3));
		dilate(foreground, foreground, kernel, cv::Point(-1,-1), iterations);
		erode(foreground, foreground, kernel, cv::Point(-1,-1), iterations);
	}

	void ForegroundProcessor::erodeBinMap(Mat foreground, int iterations)
	{
		cv::Mat kernel;
		kernel = getStructuringElement( MORPH_CROSS, Size(3, 3));
		erode(foreground, foreground, kernel, cv::Point(-1,-1), iterations);
	}

	void ForegroundProcessor::dilateBinMap(Mat foreground, int iterations)
	{
		cv::Mat kernel;
		kernel = getStructuringElement( MORPH_CROSS, Size(3, 3));
		dilate(foreground, foreground, kernel, cv::Point(-1,-1), iterations);
	}

}
