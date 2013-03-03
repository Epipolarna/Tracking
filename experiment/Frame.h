#ifndef __FRAME__
#define __FRAME__

#include <opencv2/core/core.hpp>
#include <vector>

using namespace cv;

class Frame{
public:
	Mat image;
	Mat *probabilityMap;

	Frame(Mat img);
};

#endif