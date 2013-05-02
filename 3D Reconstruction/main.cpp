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
#include "NonLinear.h"

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
	for (int i = 0; i < inlierMask.size(); ++i)
	{
		if(inlierMask[i] != 0)
		{
			inliers1.push_back(points1[i]);
			inliers2.push_back(points2[i]);
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
	
	//Actual call to the nonlinear part of the goldstandard estimation
	F = nonlin.goldNonLin(F, P1, P2, normalized3Dpoints,inliers1,inliers2);

	cout << "Refined Fundamental Matrix: " << F/F.at<double>(2,2) << std::endl;
	cout << "det(F): " << determinant(F) << std::endl;
	return F/F.at<double>(2,2);

}

int main()
{
	namedWindow("win1");
	namedWindow("win2");
	
	// Read images into a vector
	Mat image1 = imread("images/viff.000.ppm");
	Mat image2 = imread("images/viff.001.ppm");
	vector<Mat> imageList;
	imageList.push_back(image1);

	imshow("win1", image1);
	imshow("win2", image2);
	//waitKey(0);

	// FEATURES
	
	// ---------- HARRIS detector -----------
	int		maxNumberOfFeatures =	100;		// Maximum number of features to return
	double	qualityLevel =			0.01;		// Remove features with worse than 10 % of the best eigenvalue,			0.01
	double	minDistance =			30;			// Minimal Euclidiean distance between features,						3
	int		blockSize =				7;			// Size of averaging mask when calculating neighbourhood covariance,	3
	bool	useHarris =				true;		// Use Harris or die,													false
	double	k =						0.04;		// Parameter for Harris detector: Har(x,y) = det - k * trace^2,			0.04
	GoodFeaturesToTrackDetector featureDetector(maxNumberOfFeatures, qualityLevel, minDistance, blockSize, useHarris, k);
	

	/*
	// ---------- SIFT detector -----------
	double siftThreshold =		0.04;
	double siftEdgeThreshold =	10;

	SiftFeatureDetector featureDetector(siftThreshold, siftEdgeThreshold);
	*/

	vector<KeyPoint> keypoints1;
	vector<KeyPoint> keypoints2;
	vector<Point> points1;
	vector<Point> points2;

	featureDetector.detect(image1, keypoints1);
	featureDetector.detect(image2, keypoints2);




	Mat featureImage1;
	Mat featureImage2;
	drawKeypoints(image1, keypoints1, featureImage1);
	drawKeypoints(image2, keypoints2, featureImage2);

	imshow("win1", featureImage1);
	imshow("win2", featureImage2);
	waitKey(0);

	keyPoints2Points(keypoints1, points1);
	keyPoints2Points(keypoints1, points2);
	
	
	// DESCRIPTORS

	/*
	// ---------- BRIEF Descriptor -----------
	int descriptorSize = 64;	// Size of descriptor = 16, 32 or 64
	BriefDescriptorExtractor descriptorExtractor(descriptorSize);
	*/

	// ---------- SIFT Descriptor -----------
	double magnification =		3;			// 
    bool isNormalize =			true;		//
    bool recalculateAngles =	true;		//
	SiftDescriptorExtractor descriptorExtractor(magnification, isNormalize, recalculateAngles);

	Mat descriptors1;
	Mat descriptors2;
	descriptorExtractor.compute(image1, keypoints1, descriptors1);
	descriptorExtractor.compute(image2, keypoints2, descriptors2);

	//waitKey(0);

	// Match descriptors between images
	BFMatcher matcher(NORM_L2, true);
	vector<DMatch> matches;
	matcher.match(descriptors1, descriptors2, matches);
	//matches.erase(matches.begin() + 20, matches.end());
	
	
	/////////// begin Goldstandard ///////////////////
	

	// Create the 3D points











	// gör om matches till ett vettigt format.
	vector<Point2d> bestPoints1;
	vector<Point2d> bestPoints2;
	Mat F;
	for (int k=0; k<matches.size(); k++)
	{
		bestPoints1.push_back(keypoints1[matches[k].queryIdx].pt);
        bestPoints2.push_back(keypoints2[matches[k].trainIdx].pt);
	}

	// Gold standard F. points have to be ordered with correspondances at the same indices.
	F = getGoldStandardF(bestPoints1,bestPoints2);
	
	
	Mat matchImage;
	drawMatches(image1, keypoints1, image2, keypoints2, matches, matchImage);

	imshow("win1", matchImage);
	waitKey(0);

	return 0;
}




