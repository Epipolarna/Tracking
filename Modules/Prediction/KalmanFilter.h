#ifndef _KALMAN_FILTER_H_
#define _KALMAN_FILTER_H_

#include "../Frame.h"

namespace Prediction
{
	class KalmanFilter
	{
	public:
		void predict(Frame & frame);
	};
	// Additional function-/datastructuredeclarations here
}

#endif