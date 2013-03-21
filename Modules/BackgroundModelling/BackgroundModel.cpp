#include "BackgroundModel.h"

string getImgType(int imgTypeInt)
{
    int numImgTypes = 35; // 7 base types, with five channel options each (none or C1, ..., C4)

    int enum_ints[] =       {CV_8U,  CV_8UC1,  CV_8UC2,  CV_8UC3,  CV_8UC4,
                             CV_8S,  CV_8SC1,  CV_8SC2,  CV_8SC3,  CV_8SC4,
                             CV_16U, CV_16UC1, CV_16UC2, CV_16UC3, CV_16UC4,
                             CV_16S, CV_16SC1, CV_16SC2, CV_16SC3, CV_16SC4,
                             CV_32S, CV_32SC1, CV_32SC2, CV_32SC3, CV_32SC4,
                             CV_32F, CV_32FC1, CV_32FC2, CV_32FC3, CV_32FC4,
                             CV_64F, CV_64FC1, CV_64FC2, CV_64FC3, CV_64FC4};

    string enum_strings[] = {"CV_8U",  "CV_8UC1",  "CV_8UC2",  "CV_8UC3",  "CV_8UC4",
                             "CV_8S",  "CV_8SC1",  "CV_8SC2",  "CV_8SC3",  "CV_8SC4",
                             "CV_16U", "CV_16UC1", "CV_16UC2", "CV_16UC3", "CV_16UC4",
                             "CV_16S", "CV_16SC1", "CV_16SC2", "CV_16SC3", "CV_16SC4",
                             "CV_32S", "CV_32SC1", "CV_32SC2", "CV_32SC3", "CV_32SC4",
                             "CV_32F"  "CV_32FC1", "CV_32FC2", "CV_32FC3", "CV_32FC4",
                             "CV_64F", "CV_64FC1", "CV_64FC2", "CV_64FC3", "CV_64FC4"};

    for(int i=0; i<numImgTypes; i++)
    {
        if(imgTypeInt == enum_ints[i]) return enum_strings[i];
    }
    return "unknown image type";
}

namespace BackgroundModelling
{
	void BackgroundModel::update(std::list<Frame> &frames)
	{
		Frame * currFrame = &frames.front();
		Frame * prevFrame;
		
		if(frames.size() > 2)
			prevFrame = &(*(++frames.begin()));
		else
			prevFrame = NULL;

		//and create the acual background model
		ProbabilityMap *backgroundModel = new ProbabilityMap(prevFrame,currFrame);
		currFrame->probMap = backgroundModel->pImage;
		currFrame->bgModel = backgroundModel;

		cout << "first few positions are:";
		cout << currFrame->probMap.at<float>(1,0) << ", ";
		cout << currFrame->probMap.at<float>(2,0) << ", ";
		cout << currFrame->probMap.at<float>(3,0) << endl;

		cout << "first few w is: ";
		cout << currFrame->bgModel->biggestW[0] << ", ";
		cout << currFrame->bgModel->biggestW[4] << ", ";
		cout << currFrame->bgModel->biggestW[20] << ", ";
		cout << endl;

		cout << "the first gauss has sigma:";
		cout << (int)currFrame->bgModel->distributions[0].sigma[0] << ", ";
		cout << (int)currFrame->bgModel->distributions[1].sigma[0] << ", ";
		cout << (int)currFrame->bgModel->distributions[2].sigma[0] << ", ";
		cout << endl << endl << endl;

	}
	// Additional function-/methodimplementations here
}