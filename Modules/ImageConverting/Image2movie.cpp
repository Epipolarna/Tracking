#include "Modules/FrameList.h"
#include "Modules/BackgroundModelling/BackgroundModel.h"
#include "Modules/BackgroundModelling_simple/BackgroundModel_simple.h"
#include "Modules/ForegroundProcessing/ForegroundProcessor.h"
#include "Modules/ObjectIdentification/Identification.h"
#include "Modules/Prediction/Kalman.h"
#include "Modules/Evaluation/Evaluation.h"

#include "Modules/Profiler.h"


int main()
{
	CvVideoWriter *writer = 0;
    int isColor = 1;
    int fps     = 20;  // or 30
    IplImage* img = 0; 
	
	// Get format
    img=cvLoadImage("SVN/images/Renova_20080618_083045_Cam10_0002_00001.jpg");
    int frameW  = img->width; //640; // 744 for firewire cameras
    int frameH  = img->height; //480; // 480 for firewire cameras

    writer=cvCreateVideoWriter("out.avi",-1, fps,cvSize(frameW,frameH),1);

    cvWriteFrame(writer, img);      // add the frame to the file

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

		fileName = "SVN/images/Renova_20080618_083045_Cam10_0002_" + fileNumber + ".jpg";

        img=cvLoadImage((char*)fileName.c_str());

        cvWriteFrame(writer, img);

    }
    cvReleaseVideoWriter(&writer);

	return 0;
}