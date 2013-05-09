//#include "stdafx.h"


#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif


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

NonLinear::NonLinear nonlin;



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
	//namedWindow("win1");
	//((namedWindow("win2");
	
	// Read images into a vector
	//vector<Mat> imageList;
	//readImages(&imageList);

	vector<vector<pointPair>> matchesVector;
	//findMatches(&imageList, &matchesVector);

	//saveMatches(&matchesVector, "data.alx");
	loadMatches(&matchesVector, "data.alx");

	vector<Point2d> bestPoints1;
	vector<Point2d> bestPoints2;
	vector<Point2d> bestPoints21;
	vector<Point2d> bestPoints22;

	for (vector<pointPair>::iterator i = matchesVector.begin()->begin(); i < matchesVector.begin()->end(); i++)
	{
		bestPoints1.push_back(i->p1);
		bestPoints2.push_back(i->p2);
	}
	
	GoldStandardOutput GO;

	// Tiger
	//--------------------
	Estimate3D dinosaurModel;

	double Kdata[9] = {	3217.328669180762, -78.606641008226180, 289.8672403229193,
							0,					2292.424143977958,  -1070.516234777778,
							0,					0,					1};
	cv::Mat K = cv::Mat(3,3,CV_64FC1,Kdata);
	NonLinear::NonLinear nonlin(K);
	dinosaurModel.init(bestPoints1, bestPoints2, K);
	dinosaurModel.saveToFile("iteration1.alx");


	clock_t t;
	t = clock();
	nonlin.BundleAdjust(dinosaurModel.cameras, &dinosaurModel.visible3DPoint);
	std::cout << "Bundle adjustment time: " << ((float)(clock() - t))/CLOCKS_PER_SEC << std::endl;
	
	/* Taffligt försök till att läsa fler bilder.
	vector<Point2d> alxPoints1;
	vector<Point2d> alxPoints2;
	int imCounter = 1;
	do
	{
		
		for (vector<pointPair>::iterator i = matchesVector[imCounter].begin(); i < matchesVector[imCounter].end(); i++)
		{
			alxPoints1.push_back(i->p1);
			alxPoints2.push_back(i->p2);

			//cout << i->p1 << endl;
			//cout << i->p2 << endl;
		}
		dinosaurModel.addView(alxPoints1, alxPoints2);
		nonlin.BundleAdjust(dinosaurModel.cameras, &dinosaurModel.visible3DPoint);
		
		imCounter++;
	} while(imCounter < 9);*/
	
	Camera * cam2 = dinosaurModel.cameras.back();
	cout << "P2:\n " << cam2->P << "\n";
	cout << "K:\n " << cam2->K << "\n";
	cout << "R:\n " << cam2->R << "\n";
	cout << "t:\n " << cam2->t << "\n";
	vis::Visualizer v = vis::Visualizer();
	vector<Visible3DPoint> pvector = dinosaurModel.visible3DPoint;
	for(vector<Visible3DPoint>::iterator it = pvector.begin(); it != pvector.end(); ++it){
		cv::Point3d* whatIsThePoint = it->point3D;
		v.addPoint(whatIsThePoint->x,whatIsThePoint->y,whatIsThePoint->z);
		//cout << *whatIsThePoint << endl;
	}

	v.mainLoop();

	waitKey(0);

	return 0;
}




