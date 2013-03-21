#ifndef _BACKGROUND_MODEL_SIMPLE_H_
#define _BACKGROUND_MODEL_SIMPLE_H_

#include "../Frame.h"
#include "ProbabilityMap.h"
#include <iostream>
using namespace std;

namespace BackgroundModelling_simple
{
	class BackgroundModel
	{
	public:
		BackgroundModel() { varianceTreshold = 20; MAX_KERNEL_LENGTH = 10; counter = 0; }
		void update(std::list<Frame> &frames);
		void display(std::string windowID);
		
	private:
		int MAX_KERNEL_LENGTH;
		int counter;
		float varianceTreshold;
		Mat probabilityMap;
		Mat probabilityMapTemp;
	};
}

#endif