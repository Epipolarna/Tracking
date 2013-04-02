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

	void ForegroundProcessor::init(int iterations, double minDist, double minArea, double minQuotient, bool shadowSuppression)
	{
		this->threshval = 192;
		this->iterations = iterations;
		this->minDist = minDist;
		this->minArea = minArea; 
		this->minQuotient =	minQuotient;
		this->frameCounter = 0;
		this->shadows = shadowSuppression;
	}

	void ForegroundProcessor::initShadow(double tau_H = 0.1, double tau_S = 1, double alpha = 0.5, double beta = 0.99)
	{
		this->tau_H = tau_H;
		this->tau_S = tau_S;
		this->alpha = alpha;
		this->beta = beta;
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
		
		//Demo recording
		Mat demoTemp;
		cvtColor(frame.foreground, demoTemp, CV_GRAY2BGR, 3);
		demoTemp.copyTo(frame.demoImage(Range(0, frame.image.rows), 
										Range(0, frame.image.cols)));
		
		if (shadows)
		{
			suppressShadows(frame, minArea, minQuotient);
		}
		//Demo Recording
		cvtColor(frame.foreground, demoTemp, CV_GRAY2BGR, 3);
		demoTemp.copyTo(frame.demoImage(Range(0, frame.image.rows), 
										Range(frame.image.cols, frame.image.cols*2)));

		//Debug
		imshow( "Shadow Debug", frame.foreground );
		
		//Remove gray pixels
		threshMap(frame.foreground, threshval);

		distanceFilter(frame, minDist);
		dilateBinMap(frame.foreground, iterations);
		
		//Demo recording
		cvtColor(frame.foreground, demoTemp, CV_GRAY2BGR, 3);
		demoTemp.copyTo(frame.demoImage(Range(frame.image.rows, frame.image.rows*2), 
										Range(0, frame.image.cols)));		
		
		getObjects(frame);
	
		return;
	}

	void ForegroundProcessor::segmentForegroundArea(Frame & frame)
	{
		threshMap(frame.foreground, threshval);
		
		openingBinMap(frame.foreground, iterations);
		dilateBinMap(frame.foreground, iterations);
		closingBinMap(frame.foreground, iterations);

		getObjectsArea(frame, minArea, minQuotient);
	
		return;
	}
	

	void ForegroundProcessor::segmentForegroundShadow(Frame & frame)
	{
		
		threshMap(frame.foreground, threshval);
		
		if (shadows)
		{
			suppressShadows(frame, minArea, minQuotient);
		}
		
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

	////////////////// Shadow Suppression ////////////////////////
	void ForegroundProcessor::suppressShadows(Frame & frame, double minArea, double minDist)
	{
		
		//Create "most probable background"
		if (shadowModel.empty())
			shadowModel =  Mat::zeros(frame.image.size(), CV_8UC3);
		frameCounter++;
		if (frameCounter < 10)
		{
			shadowModel += (frame.image / 10);
			return;
		}
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
						// Parameters for shadow detection
						if (  ((abs((double)lastImage.at<Vec3b>(matPos)[0] - (double)shadowModel.at<Vec3b>(matPos)[0])/255 < tau_H) )		// HUE
							&& ( ((double)lastImage.at<Vec3b>(matPos)[1] - (double)shadowModel.at<Vec3b>(matPos)[1])/255 < tau_S)			// SATURATION
							&& ( (double)lastImage.at<Vec3b>(matPos)[2] / ((double)shadowModel.at<Vec3b>(matPos)[2] + 0.0001) > alpha)	// VALUE (ALPHA)
							&& ( (double)lastImage.at<Vec3b>(matPos)[2] / ((double)shadowModel.at<Vec3b>(matPos)[2] + 0.0001) < beta)	// VALUE (BETA)
							)
						{
							//Color gray for visualisation
							frame.foreground.at<uchar>(matPos) = 128;
						}
					} 
				}		
			}
		}
	}
	

	////////////////// Image Processing //////////////////////
	void ForegroundProcessor::distanceFilter(Frame & frame, double minDist)
	{
		Mat temp = Mat::zeros(frame.foreground.size(), frame.foreground.type());
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
			if (dist > minDist) //Draw contour only if distance is great enough.
			{		
				drawContours(temp, contours, i, Scalar(255), CV_FILLED); 	
			}
			dist = 0;
		}
		
		frame.foreground = temp;
		imshow( "Temp", frame.foreground );
		
	}
	
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
