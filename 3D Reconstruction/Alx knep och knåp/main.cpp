#include <opencv\cvaux.h>
#include <opencv\cv.h>
#include <opencv\highgui.h>
#include <opencv2\nonfree\features2d.hpp>

#include <vector>
using namespace cv;
using namespace std;
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
	
	featureDetector.detect(image1, keypoints1);
	featureDetector.detect(image2, keypoints2);

	Mat featureImage1;
	Mat featureImage2;
	drawKeypoints(image1, keypoints1, featureImage1);
	drawKeypoints(image2, keypoints2, featureImage2);

	imshow("win1", featureImage1);
	imshow("win2", featureImage2);
	waitKey(0);


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
	
	Mat matchImage;
	drawMatches(image1, keypoints1, image2, keypoints2, matches, matchImage);

	imshow("win1", matchImage);
	waitKey(0);

	return 0;
}

