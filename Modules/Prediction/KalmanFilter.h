#ifndef _KALMAN_FILTER_H_
#define _KALMAN_FILTER_H_

namespace Prediction
{
	class KalmanFilter
	{
	public:
		void predict(Frame & frame);
	};
}

#endif