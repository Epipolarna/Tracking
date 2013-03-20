#ifndef __PROBABILITYMAP__
#define __PROBABILITYMAP__

#include <opencv2/core/core.hpp>
#include <math.h>
#include <iostream>

#include "../Frame.h"

class Frame;

using namespace cv;
using namespace std;

struct gauss3D{
	uchar sigma[3];
	uchar mean[3];
	float w;
};

class ProbabilityMap{
public:
	Mat pImage;
	float *biggestW;

	ProbabilityMap(Frame *prevFrame, Frame *currFrame);
	void setB(int rows, int cols);

	static const int numGauss = 3; //how many distributions will be used
	const float lambda; //treshold for belonging to a distribution
	const float initSigma; //inital sigma for new distribution
	const float alpha;
	
	gauss3D *distributions;
	void updateDistributions(Mat image);
	void resetDistance(float *d);
	bool betterMatch(gauss3D bestDist, gauss3D distK);
	void initDistribution(gauss3D &g, uchar sigma, Vec3b mean);
	float sigmaSize(gauss3D g);
	float sumW(int row, int col, int maxRow);
};

#endif