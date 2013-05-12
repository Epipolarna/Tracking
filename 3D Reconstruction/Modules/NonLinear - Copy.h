#ifndef _NON_LINEAR_H_
#define _NON_LINEAR_H_



#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <libs/levmar/levmar.h> //<<<---- numera på sven.
#include "Camera.h"
#include "Estimate3D.h"
#include <fstream>

extern std::ofstream errorFile;
extern std::ofstream logFile;

namespace NonLinear
{
	
	struct goldStandardData
	{
		int nPoints;
		cv::Mat F;
		cv::Mat points1; 
		cv::Mat points2; 
	};
	
	struct goldNonLinData
	{
		int nPoints2D;
		int nPoints3D;

		std::vector<double>* params;

		
		
		cv::Mat F;
		cv::Mat K;
		std::vector<cv::Point2d> points1; 
		std::vector<cv::Point2d> points2; 
		cv::Mat points3D;
		cv::Mat C1;
		cv::Mat C2;
	};

	struct BAData
	{
		int nViews;
		int n3DPoints;
		
		double oneData;
		cv::Mat one;
		cv::Mat R;
		cv::Mat rVec;
		cv::Mat t;
		cv::Mat point2D;
		cv::Mat point3D;
		cv::Mat C;
		cv::Mat P;		

		cv::Mat K;
		std::vector<cv::Point3f> float2Ps;
		std::vector<cv::Point3f> float3DPs;
		std::vector<cv::Mat> rotations;
		std::vector<cv::Mat> translations;
		std::vector<std::vector<cv::Point2d>*> imagePoints;
		//cv::Mat allPoints3D;
		//std::vector<std::vector<cv::Mat>> visible3Dpoints;
		std::vector<std::vector<cv::Point3d*>> points3D;
		std::vector<Visible3DPoint>* all3DPoints;
		

	};



	class NonLinear
	{
	public:
		NonLinear(cv::Mat _K);
		NonLinear() {};
		void testFunc(double* params, double* residuals);
		void goldStandardRefine(cv::Mat F, std::vector<cv::Point2d> points1, std::vector<cv::Point2d> points2);
		cv::Mat goldNonLin(cv::Mat F, cv::Mat C1, cv::Mat C2, cv::Mat point3D, std::vector<cv::Point2d> points1, std::vector<cv::Point2d> points2);
		void BundleAdjust(std::list<Camera*>& views, std::vector<Visible3DPoint>* all3DPoints);
		void PnPSolver(Camera& cam);
	private:
		// Hard-coded in constructor
		cv::Mat K;
		
		
	};

	class Quaternion
	{
	public:
		double q[4];
	public:
		Quaternion() { q[0] = q[1] = q[2] = q[3] = 0; }
		Quaternion(double q0, double q1, double q2, double q3) { q[0] = q0; q[1] = q1; q[2] = q2; q[3] = q3; }
		//Quaternion(cv::Mat Rvec);	// Rvec[0] = alpha, Rvec[1] = n1, Rvec[2] = n2, Rvec[3] = n3 (n = normalized axis)
		//Quaternion(cv::Mat quarternion);	// quarternion[0..3] = q0,q1,q2,q3
		Quaternion(cv::Mat R);	// R = 3x3 Rotation matrix
		
		void normalize();
		cv::Mat toR();	// 3x3 Rotation matrix
		cv::Mat toMat();	// 1x4 matrix [q0..q3]

		double norm();	// For internal use, should always return 1 externally
	};

}


#endif