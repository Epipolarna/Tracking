// StateSpaceModel.cpp
#include "StateSpaceModel.h"

namespace Prediction
{
	StateSpaceModel::StateSpaceModel(float _x0, float _y0)
	{
		// Default values for matrices
		// Model:
		// x(t+1) = Ax(t) + w(t),	Q = var(w(t))
		// y(t)   = Cx(t) + v(t),	R = var(v(t))

		float AData[] = {	1, 0, 1, 0,
							0, 1, 0, 1,
							0, 0, 1, 0,
							0, 0, 0, 1 };
		A = cv::Mat(4, 4, CV_32FC1, AData).clone();
	
		float CData[] = {	1, 0, 0, 0,
							0, 1, 0, 0};
		C = cv::Mat(2, 4, CV_32FC1, CData).clone();

		float PData[] = {	1, 0, 0, 0,
							0, 1, 0, 0,
							0, 0, 1, 0,
							0, 0, 0, 1 };
		P = cv::Mat(4, 4, CV_32FC1, PData).clone();

		float Qvalue = 0.1f;
		float QData[] = {	Qvalue, 0, 0, 0,
							0, Qvalue, 0, 0,
							0, 0, Qvalue, 0,
							0, 0, 0, Qvalue };
		Q = cv::Mat(4, 4, CV_32FC1, QData).clone();

		R = 0.1f;

		float x0Data[] = {_x0, _y0, 0, 0};
		xHat = cv::Mat(4, 1, CV_32FC1, x0Data).clone();
	}
}