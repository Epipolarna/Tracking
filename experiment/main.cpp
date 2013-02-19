#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

using namespace cv;
using namespace std;


int main(int argc, char** argv){

	if(argc != 2){
		cout << "Usage: display_image ImageToLoadAndDisplay" << endl;
		return -1;
	}
	
	Mat image;
	image = imread(argv[1],CV_LOAD_IMAGE_COLOR);
	if(!image.data){
		cout << "could not open or find image" << std::endl;
		return -1;
	}

	namedWindow("Display window",CV_WINDOW_AUTOSIZE);
	imshow("Display window",image);
	waitKey(0);
	return 0;
}