/* 
*	frame.cpp
*
*
*/

//#include "stdafx.h"
#include "frame.h"

using namespace cv;
using namespace std;

Frame::Frame(cv::Mat _rawFrame, cv::Mat _probMap)
	: rawFrame(_rawFrame), probMap(_probMap) {}

void Frame::drawObjects(Scalar color)
{
	for (std::vector<Object>::iterator it = objects.begin(); it != objects.end(); ++it)
	{
		rectangle(rawFrame, Point(it->boundingBox.x, it->boundingBox.y), 
							Point(it->boundingBox.x + it->width, it->boundingBox.y + it->height), 
							color, 1, 8);
		line(rawFrame, Point(it->x, it->y), Point(it->x + (int)(it->dx), it->y + (int)(it->dy)), color, 2 ,8);

		std::string objectText = "("+std::to_string(it->x)+","+std::to_string(it->y)+") id:"+std::to_string(it->id);
		int fontFace = CV_FONT_HERSHEY_COMPLEX;
		double fontScale = 0.3;
		int thickness = 0.3;
		putText(rawFrame, objectText, Point(it->boundingBox.x-it->width, it->boundingBox.y+it->height+10), fontFace, fontScale, Scalar::all(255), thickness, 8);
	}
}

void Frame::drawObjects(std::vector<Object> & objects, Scalar color)
{
	for (std::vector<Object>::iterator it = objects.begin(); it != objects.end(); ++it)
	{
		rectangle(rawFrame, Point(it->boundingBox.x, it->boundingBox.y), 
							Point(it->boundingBox.x + it->width, it->boundingBox.y + it->height), 
							color, 1, 8);
		line(rawFrame, Point(it->x, it->y), Point(it->x + (int)(it->dx), it->y + (int)(it->dy)), color, 2 ,8);

		std::string objectText = "("+std::to_string(it->x)+","+std::to_string(it->y)+") id:"+std::to_string(it->id);
		int fontFace = CV_FONT_HERSHEY_COMPLEX;
		double fontScale = 0.3;
		int thickness = 0.3;
		putText(rawFrame, objectText, Point(it->boundingBox.x-it->width, it->boundingBox.y-10), fontFace, fontScale, Scalar::all(255), thickness, 8);
	}
}

void Frame::getObjects()
{
	vector<vector<Point>> contours;
	findContours( probMap.clone(), contours, CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	
	for(unsigned int i = 0; i < contours.size(); i++)
	{		
		objects.push_back(Object(boundingRect(contours[i])));	
	}
}

void Frame::getObjectsDistMap(double minDist)
{
	vector<vector<Point>> contours;
	findContours( probMap, contours, CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	Rect objRect;
	double dist = 0;
	for(unsigned int i = 0; i < contours.size(); i++)
	{	
		objRect = boundingRect(contours[i]);
		vector<Point> contour = contours[i];
		
		//Measure distance to the contour of all pixels within the bounding box.
		for( int j = objRect.x; j < objRect.x + objRect.width; j++)
			{ for( int k = objRect.y; k < objRect.y + objRect.height; k++) 
				{ 
					dist = max(dist, pointPolygonTest(contour, Point(j, k), true)); // Calculate distance
				} 
			}
		if (dist > minDist) //Create object only if distance is great enough.
		{		
			objects.push_back(Object(objRect));	
		}
		dist = 0;
	}
}


void Frame::threshMap(int threshval)
{
	int maxVal = 255;
	threshold(probMap, probMap, threshval, maxVal, THRESH_BINARY);
}

void Frame::openingBinMap(int iterations)
{
	cv::Mat kernel;
	kernel = getStructuringElement( MORPH_CROSS, Size(3, 3));
	dilate(probMap, probMap, kernel, cv::Point(-1,-1), iterations);
	erode(probMap, probMap, kernel, cv::Point(-1,-1), iterations);
}

void Frame::closingBinMap(int iterations)
{
	cv::Mat kernel;
	kernel = getStructuringElement( MORPH_CROSS, Size(3, 3));
	dilate(probMap, probMap, kernel, cv::Point(-1,-1), iterations);
	erode(probMap, probMap, kernel, cv::Point(-1,-1), iterations);
}


/////////////////////////////////////////////// Private Functions //////////////////////////////////////////////////////

void Frame::erodeBinMap(int iterations)
{
	cv::Mat kernel;
	kernel = getStructuringElement( MORPH_CROSS, Size(3, 3));
	erode(probMap, probMap, kernel, cv::Point(-1,-1), iterations);
}

void Frame::dilateBinMap(int iterations)
{
	cv::Mat kernel;
	kernel = getStructuringElement( MORPH_CROSS, Size(3, 3));
	dilate(probMap, probMap, kernel, cv::Point(-1,-1), iterations);
}

/////////////////////////////////////////////// Showimage //////////////////////////////////////////////////////

void Frame::showObjects()
{
	for (std::vector<Object>::iterator it = objects.begin(); it != objects.end(); ++it)
	{
		it->info();
	}
}

void Frame::showImageRaw(string windowID)
{
	namedWindow( windowID, CV_WINDOW_AUTOSIZE );		// Create a window for display.
    imshow( windowID, rawFrame );						// Show our image inside it.
}

void Frame::showImageProbMap(string windowID)
{
	namedWindow( windowID, CV_WINDOW_AUTOSIZE );		// Create a window for display.
    imshow( windowID, probMap );						// Show our image inside it.
}





