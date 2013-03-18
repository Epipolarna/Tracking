#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <string>
#include <list>
#include "frame.h"
#include "Identification.h"

using namespace cv;
using namespace std;

const std::string path = "C:\\Users\\TiXiR\\Documents\\skola\\TSBB15 - Computer Vision\\Project 1 - Tracking\\repository\\data\\";


int main()
{
	std::list<Frame> frames;
	Identification::generate_testdata(frames);
	
	cvNamedWindow("Identify::Test", 1 ); 

	string text = "Funny text inside the box";
	int fontFace = CV_FONT_HERSHEY_COMPLEX;
	double fontScale = 0.5;
	int thickness = 1;

	int n = 0;
	for(std::list<Frame>::iterator f = frames.begin(); f != frames.end(); f++)
	{
		n++;
		text = "[Frame "+std::to_string(n)+"("+std::to_string(frames.size())+")]";
		f->drawObjects(cv::Scalar(250, 0, 0, 255));
		putText(f->rawFrame, text, Point(5, 15), fontFace, fontScale, Scalar::all(255), thickness, 8);
		imshow("Identify::Test",f->rawFrame);
		waitKey(0);
	}
	
	waitKey(0);
	return 0;
}