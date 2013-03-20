#include "BackgroundModel.h"

namespace BackgroundModelling
{
	void BackgroundModel::update(std::list<Frame> &frames)
	{
		Frame * currFrame = &frames.front();
		Frame * prevFrame;
		
		if(frames.size() > 2)
			prevFrame = &(*(++frames.begin()));
		else
			prevFrame = NULL;

		//and create the acual background model
		ProbabilityMap *backgroundModel = new ProbabilityMap(prevFrame,currFrame);
		currFrame->probMap = backgroundModel->pImage;
		currFrame->bgModel = backgroundModel;
	}
	// Additional function-/methodimplementations here
}