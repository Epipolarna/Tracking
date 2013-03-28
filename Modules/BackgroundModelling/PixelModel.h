#ifndef __PIXELMODEL__
#define __PIXELMODEL__

#include <opencv2/core/core.hpp>
#include <math.h>
#include <iostream>
#include <cstdio>


#define NUM_CHANNELS 3
#define NUM_DISTRIBUTIONS 3

using namespace cv;
using namespace std;

struct Gauss{
	double mean[NUM_CHANNELS];
	double sigma[NUM_CHANNELS];
	double w;
	bool isBackground;
	bool wasMatched;
	int distributionID;
	int wasUpdated;
};

struct PixelModelData{
	Gauss models[NUM_DISTRIBUTIONS];
};

class PixelModel{
public:
	PixelModel(int maxRow, int maxCol);

	double lambda;
	double alpha;
    double minSigma;
    double maxW;
//private:
	int rows,cols;
	double backgroundThreshold;
	PixelModelData *dists;
	Gauss* getPixelGauss(int row, int col);
	Gauss* getPixelGauss(int row, int col,int k);
	void initGauss(Gauss *g, double *sigma, double *mean, double w);
	void initModel();
	double sigmaSize(int row, int col, int k);
	void updateModel(Mat image);
	void updatePixel(int r,int c,Mat image);
	void updatePixelGauss(int r,int c,int k,Mat image);
	void normalisePixelGausses(int r, int c);
	void sortPixelGauss(int row,int col);
	bool compare(Gauss *g1, Gauss *g2);
	void copyGauss(Gauss *s, Gauss *d);
	void markBackground(int row, int col);
	void logPixel(int row, int col);
	double distanceToGauss(Gauss *g, cv::Vec3b pixel);
	Mat getPmat(Mat image);

	//debug
	void dumpPixel(int row, int col, Mat image);
};


#endif