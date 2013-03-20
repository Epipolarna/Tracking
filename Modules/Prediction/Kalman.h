#ifndef _KALMAN_H_
#define _KALMAN_H_

#include "../Frame.h"

namespace Prediction
{
	class Kalman
	{
	public:
		Kalman(float x0 = 0, float y0 = 0);

		void predict(Frame & frame);

		cv::Mat A, C, Q, P, xHat;
		float R;
	};
	// Additional function-/datastructuredeclarations here
}

#endif