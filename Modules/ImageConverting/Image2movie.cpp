
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

#include <string>
#include <iostream>



using namespace std;
using namespace cv;

int main()
{

    int isColor = 1;
    int fps     = 20;  // or 30
    Mat img;
	Size imgSize;

	img = imread("Renova_20080420_083025_Cam10_0000/dataRenova_20080420_083025_Cam10_0000_00001.jpg");
	imgSize = img.size();
	VideoWriter video("out.mpeg", CV_FOURCC('P','I','M','1'), fps, imgSize);

	video << img;

	string fileName, fileNumber;
	int zeros = 0;
	int endingNumber = 1500;

    for(int fileNo = 0; fileNo <= endingNumber ;fileNo++){
        //FirstFile=fF;   
		fileNumber =  to_string(fileNo);

		zeros = 5 - fileNumber.size();

		for (int i = 0; i < zeros; i++)
		{
			fileNumber = "0" + fileNumber;
		}
		
		cout << "Image number: " << fileNumber << endl;

		fileName = "Renova_20080420_083025_Cam10_0000/dataRenova_20080420_083025_Cam10_0000_" + fileNumber + ".jpg";

		img = imread(fileName);

		video << img;

    }

	return 0;
}