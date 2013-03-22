/* 
*	frame.cpp
*/

#include "Frame.h"

using namespace cv;
using namespace std;

Frame::Frame(cv::Mat image,cv::Mat probMap) : image(image),probMap(probMap)  {}

void Frame::drawObjects(Scalar color)
{
	for (std::vector<Object>::iterator it = objects.begin(); it != objects.end(); ++it)
	{
		if(it->lost)
			color = Scalar(255, 255, 0);

		rectangle(image, Point(it->boundingBox.x, it->boundingBox.y), 
							Point(it->boundingBox.x + it->width, it->boundingBox.y + it->height), 
							color, 1, 8);
		line(image, Point(it->x, it->y), Point(it->x + (int)(it->dx), it->y + (int)(it->dy)), color, 2 ,8);

		std::string objectText = "("+std::to_string(it->x)+","+std::to_string(it->y)+") id:"+std::to_string(it->id);
		int fontFace = CV_FONT_HERSHEY_COMPLEX;
		double fontScale = 0.3;
		int thickness = 0.3;
		putText(image, objectText, Point(it->boundingBox.x-it->width/2, it->boundingBox.y+it->height+10), fontFace, fontScale, Scalar::all(255), thickness, 8);
	}
}

void Frame::drawObjectsPrediction(Scalar color)
{
	for (std::vector<Object>::iterator it = objects.begin(); it != objects.end(); ++it)
	{
		if(it->lost)
			color = Scalar(255, 255, 0);

		rectangle(image, Point(it->xHat, it->yHat), 
							Point(it->xHat, it->yHat),
							color, 1, 8);
		line(image, Point(it->xHat, it->yHat), Point(it->xHat + (int)(it->dx), it->yHat + (int)(it->dy)), color, 2 ,8);

		std::string objectText = "("+std::to_string(it->xHat)+","+std::to_string(it->yHat)+") id:"+std::to_string(it->id);
		int fontFace = CV_FONT_HERSHEY_COMPLEX;
		double fontScale = 0.3;
		int thickness = 0.3;
		putText(image, objectText, Point(it->xHat-it->width/2, it->yHat+it->height+10), fontFace, fontScale, Scalar::all(255), thickness, 8);
	}
}

void Frame::drawObjects(std::vector<Object> & objects, Scalar color)
{
	for (std::vector<Object>::iterator it = objects.begin(); it != objects.end(); ++it)
	{
		rectangle(image, Point(it->boundingBox.x, it->boundingBox.y), 
							Point(it->boundingBox.x + it->width, it->boundingBox.y + it->height), 
							color, 1, 8);
		line(image, Point(it->x, it->y), Point(it->x + (int)(it->dx), it->y + (int)(it->dy)), color, 2 ,8);

		std::string objectText = "("+std::to_string(it->x)+","+std::to_string(it->y)+") id:"+std::to_string(it->id);
		int fontFace = CV_FONT_HERSHEY_COMPLEX;
		double fontScale = 0.3;
		int thickness = 0.3;
		putText(image, objectText, Point(it->boundingBox.x-it->width/2, it->boundingBox.y-10), fontFace, fontScale, Scalar::all(255), thickness, 8);
	}
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
    imshow( windowID, image );						// Show our image inside it.
}

void Frame::showImageProbMap(string windowID)
{
	namedWindow( windowID, CV_WINDOW_AUTOSIZE );		    // Create a window for display.
    imshow( windowID, probMap );						// Show our image inside it.
}





