/* TSBB15 Tracking project 2013
 * - - - - - - - - - - - - - - -
 * Principal contributer: Alexander Sj'o'holm
 */

#ifndef _KALMAN_H_
#define _KALMAN_H_

#include "../Frame.h"
#include <functional>
#include <numeric>

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
}

#endif