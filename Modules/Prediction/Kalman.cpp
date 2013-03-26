#include "Kalman.h"
//#include <vector>
#include <functional>
#include <numeric>

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
			float yData[] = {(float)i->x, (float)i->y};
			y = Mat(2, 1, CV_32FC1, yData);

			// If object is not found in current frame perform only Time Update
			if (i->isChild != true && i->isLost != true)
			{
				MeasurementUpdate(i);
			}
			TimeUpdate(i);

			xHat = i->model.xHat.ptr<float>();
			i->xHat = xHat[0];
			i->yHat	= xHat[1];




			// Smooth velocity with 5 latest measurements
			i->model.dxHist.push_front(xHat[2]);
			i->model.dyHist.push_front(xHat[3]);

			if (i->model.dxHist.size() > i->model.smoothingHistoryAmount)
			{
				i->model.dxHist.pop_back();
				i->model.dyHist.pop_back();
			}

			float dxMean = 0;
			float dyMean = 0;
			float sampleAmount = i->model.smoothingHistoryAmount;
			
			for (list<float>::iterator j = i->model.dxHist.begin(); j != i->model.dxHist.end(); j++)
			{
				dxMean += *j;
			}
			for (list<float>::iterator j = i->model.dyHist.begin(); j != i->model.dyHist.end(); j++)
			{
				dyMean += *j;
			}
			/*
			if(!i->model.hasConverged)
			{
				if(i->model.preConvergenceIteration < i->model.smoothingHistoryAmount)
				{
					sampleAmount = i->model.preConvergenceIteration;
					i->model.preConvergenceIteration++;
				}
				else
					i->model.hasConverged = true;

				dxMean = xHat[2];
				dyMean = xHat[3];
				std::cout << "hasNotConverged!!!\n";
			}
			else
			{
			}*/
				dxMean = dxMean/sampleAmount;
				dyMean = dyMean/sampleAmount;

			if (i->model.dxHist.size() == i->model.smoothingHistoryAmount)
			{
				i->dx = dxMean;
				i->dy = dyMean;
			}

			if (i->isChild == true || i->isLost == true)
			{
				i->x = (int)floor(i->xHat + 0.5);
				i->y = (int)floor(i->yHat + 0.5);
			}
			//cout << i->model.P << endl;
			//waitKey(0);
		}
	}
	// Additional function-/methodimplementations here
}