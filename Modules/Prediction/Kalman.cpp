#include "Kalman.h"
//#include <vector>

namespace Prediction
{
	void Kalman::MeasurementUpdate(std::vector<Object>::iterator i)
	{
		// Some auxilary variables are calculated to save computations.
		// Calculate an auxilary L
		L = i->model.P*i->model.C.t();
		// Calculate an auxilary S
		S = i->model.C*L + i->model.R;
		// Calculate an auxilary K, often refered to as Kalman Gain. Can be adjusted initially by P, and by Q over time.
		K = L*S.inv();

		i->model.xHat = i->model.xHat + K*(y - i->model.C*i->model.xHat);
		i->model.P = i->model.P - K*L.t();
	}

	void Kalman::TimeUpdate(std::vector<Object>::iterator i)
	{
		i->model.xHat = i->model.A*i->model.xHat;
		i->model.P = i->model.A*i->model.P*i->model.A.t() + i->model.Q;
	}

	void Kalman::predict(Frame & frame)
	{

		for(std::vector<Object>::iterator i = frame.objects.begin(); i != frame.objects.end(); i++)
		{
			float yData[] = {i->x, i->y};
			y = Mat(2, 1, CV_32FC1, yData);

			// If object is lost in current frame perform only Time Update
			if (i->lost != true)
			{
				MeasurementUpdate(i);
			}
			TimeUpdate(i);

			// Update values in current object according to Kalman
			xHat = i->model.xHat.ptr<float>();
			i->xHat = xHat[0];
			i->yHat	= xHat[1];
			i->dx = xHat[2];
			i->dy = xHat[3];
		}
	}
	// Additional function-/methodimplementations here
}