//#include "stdafx.h"

#include <math.h>
#include <time.h>
#include <opencv\cvaux.h>
#include <opencv\cv.h>
#include <opencv\highgui.h>
#include <opencv2\nonfree\features2d.hpp>
#include <libs/levmar/levmar.h> //<<<---- numera på sven.
#include <iostream>
#include "Modules/NonLinear.h"
#include "Modules\correspondanceExtractor.h"
#include "Modules\Camera.h"
#include "Modules\Estimate3D.h"
#include "Modules\Visualization\Visualizer.h"

#include <vector>
using namespace cv;
using namespace std;

#define mFileName(stringSource, name, version, subversion, filetype) { std::stringstream ss; ss << name << version << '.' << subversion << filetype; stringSource = ss.str(); }
#define mElapsedTime(t) ((float)(clock() - t))/CLOCKS_PER_SEC

NonLinear::NonLinear nonlin;

enum ePROGRAM_STATE
{
	CALCULATE_CORRESPONDANCES,
	ESTIMATE3D,
	LOADFROMFILE
} programState;


//Utility function used in preprocessing for the goldstandard stuff
void keyPoints2Points(vector<KeyPoint>& src, vector<Point>& dst )
{
	for(std::vector<KeyPoint>::iterator it = src.begin(); it != src.end(); ++it)
	{
		dst.push_back(it->pt);
	}
}
int main()
{
	// Variables
	//--------------
	int version = 1;
	clock_t t_BA, t_mainLoop;
	std::string fileName;
	mFileName(fileName,"iteration",version,1.1,"alx");
	double Kdata[9] = {	3217.328669180762, -78.606641008226180, 289.8672403229193,
							0,					2292.424143977958,  -1070.516234777778,
							0,					0,					1};
	cv::Mat K = cv::Mat(3,3,CV_64FC1,Kdata);
	NonLinear::NonLinear nonlin(K);
	Estimate3D dinosaurModel;
	CorrespondanceExtractor corrEx;
	vector<vector<pointPair>> matchesVector;

	// Select program state
	programState = ePROGRAM_STATE::ESTIMATE3D;
	fileName = "iteration1.2.alx";

// The main program and it's 3 states
//-----------------------------------
	if(programState == ePROGRAM_STATE::CALCULATE_CORRESPONDANCES)
	{		
		corrEx.readImages();
		corrEx.findMatches();
		corrEx.saveMatches("data.alx");
		std::cout << "Correspondances are computed and saved to file!\n";
	}
	else
	if(programState == ePROGRAM_STATE::LOADFROMFILE)
	{
		std::cout << "# Loading file \"" << fileName << "\"..\n";
		dinosaurModel.loadFromFile(fileName);
		std::cout << "# File loaded!\n";
	}
	else
	if(programState == ePROGRAM_STATE::ESTIMATE3D)
	{
		// Load matches
		corrEx.loadMatches("data.alx");

		vector<Point2d> imagePoints1;
		vector<Point2d> imagePoints2;
		corrEx.getBAPoints(0, imagePoints1, imagePoints2);

		// Pre Main loop
		//--------------------
			std::cout << "# Iteration " << 1 << " started..\n";	
			t_mainLoop = clock();
		dinosaurModel.init(imagePoints1, imagePoints2, K);
		dinosaurModel.saveToFile("iteration"+std::to_string(1)+".1.alx");
	
			std::cout << "# | Bundle adjustment started..\n";
			t_BA = clock();
		nonlin.BundleAdjust(dinosaurModel.cameras, &dinosaurModel.visible3DPoint);
			std::cout << "# | Bundle adjustment finished! \n";
			std::cout << "# |		Bundle adjustment time: " << mElapsedTime(t_BA) << " sec\n";
		dinosaurModel.saveToFile("iteration"+std::to_string(1)+".2.alx");
				std::cout << "# Iteration " << 1 << " Finished!\n";
				std::cout << "# | Main loop time: " << mElapsedTime(t_mainLoop) << " sec\n";
				std::cout << "# -----------------------------\n";
	
		// Main loop
		//--------------------	
		for(int imageCounter = 1; imageCounter < 1; imageCounter++)
		{
				std::cout << "# Iteration " << imageCounter+1 << " started..\n";
				t_mainLoop = clock();
			imagePoints1.clear();
			imagePoints2.clear();
			corrEx.getBAPoints(imageCounter, imagePoints1, imagePoints2);
			dinosaurModel.addView(imagePoints1, imagePoints2);
			dinosaurModel.saveToFile("iteration"+std::to_string(version)+".1.alx");
				std::cout << "# | Bundle adjustment started..\n";
				t_BA = clock();
			nonlin.BundleAdjust(dinosaurModel.cameras, &dinosaurModel.visible3DPoint);
				std::cout << "# |	Bundle adjustment time: " << mElapsedTime(t_BA) << " sec\n";
			dinosaurModel.saveToFile("iteration"+std::to_string(version)+".2.alx");
		
				std::cout << "# Iteration " << imageCounter+1 << " Finished!\n";
				std::cout << "# | Main loop time: " << mElapsedTime(t_mainLoop) << " sec\n";
				std::cout << "# -----------------------------\n";
		}

	}

	if(programState == LOADFROMFILE || programState == ESTIMATE3D)
	{
		std::cout << "# Starting Visualizer...\n";
		vis::Visualizer v = vis::Visualizer();
		vector<Visible3DPoint> pvector = dinosaurModel.visible3DPoint;
		for(vector<Visible3DPoint>::iterator it = pvector.begin(); it != pvector.end(); ++it){
			cv::Point3d* whatIsThePoint = it->point3D;
			v.addPoint(whatIsThePoint->x,whatIsThePoint->y,whatIsThePoint->z);
			//cout << *whatIsThePoint << endl;
		}
		std::cout << "# Visualizer finished, lets see some balls!\n";
		v.mainLoop();

	}

	waitKey(0);
	return 0;
}




