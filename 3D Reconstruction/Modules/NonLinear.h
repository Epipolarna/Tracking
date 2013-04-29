#ifndef _NON_LINEAR_H_
#define _NON_LINEAR_H_

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

//#include </libs/levmar/levmar.h> //<<<---- numera på sven.
namespace NonLinear
{
	struct goldStandardData
	{
		int nPoints;
		cv::Mat F;
		cv::Mat points1; 
		cv::Mat points2; 
	};
	
	class NonLinear
	{
	public:
		void testFunc(double* params, double* residuals);
		void goldStandardRefine(cv::Mat F, std::vector<cv::Point2d> points1, std::vector<cv::Point2d> points2);
	private:
		
	};
}


#endif