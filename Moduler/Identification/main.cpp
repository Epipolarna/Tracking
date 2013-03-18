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
	std::list<Frame> testFrames;
	Identification::generate_testdata(testFrames);
	Identification::Identifier identifier;


	cvNamedWindow("Identify::Test", 1 ); 

	string text = "Funny text inside the box";
	int fontFace = CV_FONT_HERSHEY_COMPLEX;
	double fontScale = 0.5;
	int thickness = 1;

	int tests = testFrames.size();
	int n = 0;
	while(!testFrames.empty())
	{
		n++;
		frames.push_front(testFrames.front());
		testFrames.pop_front();

		identifier.identify(frames);

		text = "[Frame "+std::to_string(n)+"("+std::to_string(tests)+")]";
		frames.front().drawObjects(cv::Scalar(250, 0, 0, 255));
		putText(frames.front().rawFrame, text, Point(5, 15), fontFace, fontScale, Scalar::all(255), thickness, 8);
		frames.front().showImageRaw("Identify::Test");
		waitKey(0);
	}
	
	waitKey(0);
	return 0;
}