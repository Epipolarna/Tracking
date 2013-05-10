#include "correspondanceExtractor.h"

using namespace std;
using namespace cv;

CorrespondanceExtractor::CorrespondanceExtractor()
{
	
}

void CorrespondanceExtractor::readImages()
{
	string fileName, fileNumber;
	string fileBeginning = "data/dinosaur/viff.";
	string fileEnding = ".ppm";

	int endingNumber = 36;

    for(int fileNo = 0; fileNo <= endingNumber ;fileNo++)
	{    
		fileNumber =  to_string(fileNo);

		int zeros = 3 - fileNumber.size();

		for (int i = 0; i < zeros; i++)
		{
			fileNumber = "0" + fileNumber;
		}

		fileName = fileBeginning + fileNumber + fileEnding;

		cout << "Filename: " << fileName << endl;
		imageList.push_back(imread(fileName));
		if(imageList.back().empty())
			cout << "\t Could not read file!\n";
    }
}

void CorrespondanceExtractor::findMatches()
{
	// FEATURES

	// ---------- HARRIS detector -----------
	int		maxNumberOfFeatures =	100;		// Maximum number of features to return
	double	qualityLevel =			0.005;		// Remove features with worse than 99 % of the best eigenvalue,			0.01
	double	minDistance =			20;			// Minimal Euclidiean distance between features,						3
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

	// DESCRIPTORS

	/*
	// ---------- BRIEF Descriptor -----------
	int descriptorSize = 64;	// Size of descriptor = 16, 32 or 64
	BriefDescriptorExtractor descriptorExtractor(descriptorSize);
	*/
		
	// ---------- SIFT Descriptor -----------
	double magnification =		3;			// 3
	bool isNormalize =			true;		// true
	bool recalculateAngles =	true;		// true 
	SiftDescriptorExtractor descriptorExtractor(magnification, isNormalize, recalculateAngles);

	// MATCHER

	BFMatcher matcher(NORM_L2, true);

	Mat image1;
	Mat image2;

	
	vector<KeyPoint> keypoints1;
	vector<KeyPoint> keypoints2;
	Mat featureImage1;
	Mat featureImage2;
	Mat descriptors1;
	Mat descriptors2;
	int maxGeometricDistance = 200;

	vector<vector<pointPair>> sequenceMatches;

	for (vector<Mat>::iterator i = imageList.begin(); i < imageList.end() - 1; i++)
	{
		
		//image1 = *i;
		//image2 = *(i+1);		
		
		//imshow("win1", image1);
		//imshow("win2", image2);
		//waitKey(0);

		//featureDetector.detect(image1, keypoints1);
		//featureDetector.detect(image2, keypoints2);
		featureDetector.detect(*i, keypoints1);
		featureDetector.detect(*(i+1), keypoints2);

		//drawKeypoints(image1, keypoints1, featureImage1);
		//drawKeypoints(image2, keypoints2, featureImage2);
		drawKeypoints(*i, keypoints1, featureImage1);
		drawKeypoints(*(i+1), keypoints2, featureImage2);

		//imshow("win1", featureImage1);
		//imshow("win2", featureImage2);
		//waitKey(0);

		//descriptorExtractor.compute(image1, keypoints1, descriptors1);
		//descriptorExtractor.compute(image2, keypoints2, descriptors2);
		descriptorExtractor.compute(*i, keypoints1, descriptors1);
		descriptorExtractor.compute(*(i+1), keypoints2, descriptors2);

		// Match descriptors between images
		vector<DMatch> rawMatches, matches;
		matcher.match(descriptors1, descriptors2, rawMatches);
		
		// Remove bad matches
		for (vector<DMatch>::iterator j = rawMatches.begin(); j < rawMatches.end(); j++)
		{
			if ((*j).distance < maxGeometricDistance)
			{
				matches.push_back(*j);
			}
		}

		//vector<Point2d> bestPoints1;
		//vector<Point2d> bestPoints2;

		vector<pointPair> pointPairs;

		for (int k=0; k<matches.size(); k++)
		{
			//bestPoints1.push_back(keypoints1[matches[k].queryIdx].pt);
			//bestPoints2.push_back(keypoints2[matches[k].trainIdx].pt);

			pointPair temp;
			temp.p1.x = keypoints1[matches[k].queryIdx].pt.x;
			temp.p1.y = keypoints1[matches[k].queryIdx].pt.y;
			temp.p2.x = keypoints2[matches[k].trainIdx].pt.x;
			temp.p2.y = keypoints2[matches[k].trainIdx].pt.y;

			//cout << "p1: " << temp.p1 << endl;
			//cout << "p2: " << temp.p2 << endl;

			pointPairs.push_back(temp);
		}

		matchesVector.push_back(pointPairs);

		//matchesVector->push_back(matches);
		cout << "matches: " << matches.size() << endl;
		
		Mat matchImage;
		drawMatches(*i, keypoints1, *(i+1), keypoints2, matches, matchImage);

		imshow("win1", matchImage);
		waitKey(1);
	}
}

void CorrespondanceExtractor::saveMatches(char* filename)
{
	ofstream os(filename, ios::binary);
	if(!os)
	{
		cout << "Error saving to file " << filename << endl;
	}

	int imageCounter = 0;

	os << "# Correspondances for each imagepair in sequnce \n";

	for (vector<vector<pointPair>>::iterator i = matchesVector.begin(); i < matchesVector.end(); i++)
	{
		imageCounter++;
		os << "# Imagepair " << imageCounter << "\n";
		for (vector<pointPair>::iterator j = i->begin(); j < i->end(); j++)
		{
			os << "p ";
			os << j->p1.x << " ";
			os << j->p1.y << " ";
			os << j->p2.x << " ";
			os << j->p2.y << " ";
			os << "\n";
		}
		os << "# Partly Done \n\n";
	}

	cout << "Successfully saved" << endl;
}

bool CorrespondanceExtractor::loadMatches(char* filename)
{
	ifstream is(filename, ios::binary);
	if(!is)
	{
		cout << "Error loading from file " << filename << endl;
		cout << "Recalculating correspondances" << endl;
		return false;
	}
	
	string line;
	istringstream ss;
	vector<pointPair> pointPairs;

	while (getline(is, line))
	{
		
		if(line.substr(0,2) == "p ")
		{
			// A pointpair

			ss = istringstream(line.substr(2));
			pointPair temp;
			ss >> temp.p1.x;
			ss >> temp.p1.y;
			ss >> temp.p2.x;
			ss >> temp.p2.y;

			pointPairs.push_back(temp);
		}
		else if(line.substr(0,13) == "# Partly Done")
		{
			// Done loadng one imagepair
			matchesVector.push_back(pointPairs);
			pointPairs.clear();
		}
		else if(line.substr(0,1) == "#")
		{
			// A comment line, ignore.
		}
		else
		{
			// Not suported line
		}
	}

	cout << "Successfully loaded" << endl;
	return true;
}

void CorrespondanceExtractor::getBAPoints(int imagePair, vector<Point2d>& BAPoints1, vector<Point2d>& BAPoints2)
{
	for (vector<pointPair>::iterator i = matchesVector[imagePair].begin(); i < matchesVector[imagePair].end(); i++)
	{
		BAPoints1.push_back(i->p1);
		BAPoints2.push_back(i->p2);
	}
}