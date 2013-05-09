#ifndef _ESTIMATE3D_H_
#define _ESTIMATE3D_H_

#include <iostream>
#include <opencv\cv.h>
#include "Camera.h"
#include "Modules/NonLinear.h"
#include <time.h>
#include <string>
#include <sstream>
#include "Parser.h"

/* Estimate3D
 * ------------
 *
 * visible3DPoint
 *	 Each contains:
 *	  - a unique 3D-point 
 *	  - a list with ObserverPairs
 *		 Each contains:
 *			- 2 cameras
 *			- a pointPair (2D)
 * 
 * cameras
 *	 Each contains:
 *	  - P,K,C,R,t
 *	  - pointPairs
 *		 Each contains:
 *			- pointPair (2D)
 *	  - point3Ds
 *		 Each contains:
 *			- point3D*  (3D)
 * 
 * cameraPair
 *	 Each contains:
 *	  - camera1, camera2
 *	  - F,E
 *	  - pointPairs
 *		 Each contains:
 *			- pointPair (2D)
 *	  - point3Ds
 *		 Each contains:
 *			- point3D*  (3D)
 *
 */

class Estimate3D
{
public:
	std::vector<Visible3DPoint> visible3DPoint;
	std::list<Camera*> cameras;
	std::vector<CameraPair> cameraPair;
	cv::Mat K;	//Internal Camera parameters
	std::vector<View> views;

	Parser parser;
public:
	Estimate3D(void);
	~Estimate3D(void);

	// p1 & p2 are corresponding 2D points, K_ is the Internal Camera matrix
	void init(cv::vector<cv::Point2d> & p1, cv::vector<cv::Point2d> & p2, cv::Mat & K_);
	void addView(cv::vector<cv::Point2d> & p1, cv::vector<cv::Point2d> & p2);

	void saveToFile(std::string path);
	void loadFromFile(std::string path);
};

// Checks is p3D is new to cam. If it isn't then it is pointed to the old one ((*p3D) = old)
bool isUnique3DPoint(Camera * cam, cv::Point2f p2D, cv::Point3d ** p3D);


cv::Mat crossop(cv::Mat vector);

/* Calculate the camera matrix C from the fundamental matrix F
 *  F - 3x3 matrix
 *  C - 3x4 matrix (with the assumption of the other camera matrix beeing the normalized camera matrix)
 */
cv::Mat cameraFromFundamentalMatrix(cv::Mat & F);

/* Apply the cross operator on a vector X or a matrix X
 *	X - 1x3 or 3x1 -> 3x3
 *  X - 3x3		   -> 1x3
 */
cv::Mat crossOperator(cv::Mat X);

struct GoldStandardOutput
{
	cv::Mat P1,P2;
	cv::vector<cv::Point2f> inlier1,inlier2;
	cv::Mat point3D;	// 3xN
};

cv::Mat getGoldStandardF(cv::vector<cv::Point2d>& points1, cv::vector<cv::Point2d>& points2, cv::Mat K_, GoldStandardOutput * Gout = 0, int RANSAC_threshold = 3);

cv::Mat normalizedCamera();




#endif
