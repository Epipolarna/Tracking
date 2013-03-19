#include "BackgroundModel.h"

namespace BackgroundModelling
{
	void BackgroundModel::update(std::list<Frame> & frames)
	{
		Frame * currFrame = &frames.front();
		Frame * prevFrame;
		if(frames.size() > 2)
			prevFrame = &(*(++frames.begin()));
		else
			prevFrame = NULL;


		//<TODO>
	}
	// Additional function-/methodimplementations here
}