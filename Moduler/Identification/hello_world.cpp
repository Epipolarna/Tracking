#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <string>
#include <list>
#include "frame.h"

using namespace cv;
using namespace std;

const std::string path = "C:\\Users\\TiXiR\\Documents\\skola\\TSBB15 - Computer Vision\\Project 1 - Tracking\\repository\\data\\";

Frame last;
Frame current;

int main()
{
	Mat image;
	image = imread(path+"logo.png",CV_LOAD_IMAGE_COLOR);
	if(!image.data){
		cout << "could not open or find image" << std::endl;
		waitKey(0);
		return -1;
	}

	rectangle(image, Point(10,10), Point(80,80), cvScalar(0, 0, 255, 0));
	namedWindow("Display window",CV_WINDOW_AUTOSIZE);
	imshow("Display window",image);
	waitKey(0);
	return 0;
}