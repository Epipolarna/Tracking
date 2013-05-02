#ifndef _NON_LINEAR_H_
#define _NON_LINEAR_H_



#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <libs/levmar/levmar.h> //<<<---- numera på sven.


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



	class NonLinear
	{
	public:
		NonLinear();
		void testFunc(double* params, double* residuals);
		void goldStandardRefine(cv::Mat F, std::vector<cv::Point2d> points1, std::vector<cv::Point2d> points2);
		cv::Mat goldNonLin(cv::Mat F, cv::Mat C1, cv::Mat C2, cv::Mat point3D, std::vector<cv::Point2d> points1, std::vector<cv::Point2d> points2);
	private:
		// Hard-coded in constructor
		cv::Mat K;
		
		
	};
}


#endif