#include "Identification.h"


namespace Identification
{
	const int cTEST_FRAME_WIDTH = 480;
	const int cTEST_FRAME_HEIGHT = 360;


	#define NEW_FRAME() frameList.push_back(Frame(cv::Mat(cTEST_FRAME_HEIGHT, cTEST_FRAME_WIDTH, CV_8UC1), cv::Mat(cTEST_FRAME_HEIGHT, cTEST_FRAME_WIDTH, CV_8UC1)));
	#define INSERT_OBJECT(x,y) frameList.back().objects.push_back(Object(x, y, 0, 0, 20, 60));

	void generate_testdata(std::list<Frame> & frameList)
	{
		int stepLength = 10;
		for(int i = 0; i < cTEST_FRAME_WIDTH; i+=stepLength)
		{
			NEW_FRAME(); INSERT_OBJECT(i, 200);
		}
	}

};