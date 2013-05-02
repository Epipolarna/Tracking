//#include "stdafx.h"


#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif


#include <math.h>
#include <opencv\cvaux.h>
#include <opencv\cv.h>
#include <opencv\highgui.h>
#include <opencv2\nonfree\features2d.hpp>
#include <libs/levmar/levmar.h> //<<<---- numera på sven.
#include <iostream>
#include "Modules/NonLinear.h"
#include "Modules\correspondanceExtractor.h"

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

Mat crossop(cv::Mat vector)
	{
		double data[9] = {	0, -vector.ptr<double>()[2], vector.ptr<double>()[1],
							vector.ptr<double>()[2], 0, -vector.ptr<double>()[0],
							-vector.ptr<double>()[1], vector.ptr<double>()[0], 0};
		Mat derp = Mat::zeros(3,3,CV_64FC1);
		derp.at<double>(0,1) = -vector.at<double>(2,0);
		derp.at<double>(0,2) = vector.at<double>(1,0);
		derp.at<double>(1,0) = vector.at<double>(2,0);
		derp.at<double>(1,2) = -vector.at<double>(0,0);
		derp.at<double>(2,0) = -vector.at<double>(1,0);
		derp.at<double>(2,1) = vector.at<double>(0,0);
		return derp;
}

// Gold standard F. points have to be ordered with correspondances at the same indices.
cv::Mat getGoldStandardF(vector<Point2d>& points1, vector<Point2d>& points2, int RANSAC_threshold = 3)
{
	
	vector<uchar> inlierMask;
	Mat P1,P2,dinoHomPoints;
	// inital Fhat
	cv::Mat F = findFundamentalMat(points1, points2, CV_FM_RANSAC, RANSAC_threshold, 0.99, inlierMask);

	//Extract the inliers in the data
	vector<Point2d> inliers1;
	vector<Point2d> inliers2;

	cout << "nu börjar de" << endl;

	for (int i = 0; i < inlierMask.size(); ++i)
	{
		if(inlierMask[i] != 0)
		{
			inliers1.push_back(points1[i]);
			inliers2.push_back(points2[i]);

			cout << points1[i] << endl;
			cout << points2[i] << endl;
		}
	}
	//Get epipolar point coordinates for P2 estimation
	SVD singval = SVD(F,SVD::FULL_UV);
	Mat eppoint2 = singval.u.col(2);
	Mat epCross2 = crossop(eppoint2);
	P1 = Mat::eye(3,4,CV_64FC1);
	// Create P2
	hconcat(epCross2 * F,eppoint2.clone(),P2);

	//Triangulate
	triangulatePoints(P1, P2, inliers1, inliers2, dinoHomPoints);
	
	Mat normalized3Dpoints = dinoHomPoints.rowRange(0,3);
	// Get 3D points as an 3-by-N Matrix for easier data access by the nonlinear module.
	for (int i = 0; i < dinoHomPoints.size().width; ++i)
	{
		normalized3Dpoints.col(i) = normalized3Dpoints.col(i)/dinoHomPoints.at<double>(3,i);
	}
	
	cout << "Inlier percentage  " << (double)inliers1.size()/(double)points1.size()*100 << endl;
	cout << "Fundamental Matrix: " << F << std::endl;
	cout << "det(F): " << determinant(F) << std::endl;
	
	nonlin.goldStandardRefine(F.clone(), inliers1,inliers2);

	
	//Actual call to the nonlinear part of the goldstandard estimation
	F = nonlin.goldNonLin(F, P1, P2, normalized3Dpoints,inliers1,inliers2);

	cout << "Refined Fundamental Matrix: " << F/F.at<double>(2,2) << std::endl;
	cout << "det(F): " << determinant(F) << std::endl;
	//return F/F.at<double>(2,2);
	cout << endl;
	cout << endl;
	nonlin.goldStandardRefine(F, inliers1,inliers2);

	cout << "Refined Fundamental Matrix: " << F/F.at<double>(2,2) << std::endl;
	cout << "det(F): " << determinant(F) << std::endl;
	return F/F.at<double>(2,2);

}

int main()
{
	namedWindow("win1");
	namedWindow("win2");
	
	// Read images into a vector
	vector<Mat> imageList;
	readImages(&imageList);

	vector<vector<pointPair>> matchesVector;
	findMatches(&imageList, &matchesVector);

	saveMatches(&matchesVector, "data.alx");
	loadMatches(&matchesVector, "data.alx");

	cout << matchesVector.begin()->begin()->p1 << endl;
	cout << matchesVector.begin()->begin()->p2 << endl;
	
	waitKey(0);

	vector<Point2d> bestPoints1;
	vector<Point2d> bestPoints2;

	for (vector<pointPair>::iterator i = matchesVector.begin()->begin(); i < matchesVector.begin()->end(); i++)
	{
		bestPoints1.push_back(i->p1);
		bestPoints2.push_back(i->p2);

		cout << i->p1 << endl;
		cout << i->p2 << endl;
	}

	Mat F;
	// Gold standard F. points have to be ordered with correspondances at the same indices.
	F = getGoldStandardF(bestPoints1,bestPoints2);

	waitKey(0);

	return 0;
}




