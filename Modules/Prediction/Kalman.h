#ifndef _KALMAN_H_
#define _KALMAN_H_

#include "../Frame.h"

namespace Prediction
{
	class Kalman
	{
	public:
		Kalman() {}

		void predict(Frame & frame);
	private:
		void MeasurementUpdate(std::vector<Object>::iterator i);
		void TimeUpdate(std::vector<Object>::iterator i);
		Mat S, L, K, y;
		float* xHat;
	};
	// Additional function-/datastructuredeclarations here
}

#endif