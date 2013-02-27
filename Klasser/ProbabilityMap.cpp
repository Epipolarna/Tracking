#include "ProbabilityMap.h"


ProbabilityMap::ProbabilityMap(vector<Frame> *prevFrames, Mat *currFrameImage){
	//assume everything is background if we have not prev data
	if(prevFrames->size() == 0){
		this->bgProbability = new Mat(currFrameImage->size->height,currFrameImage->size->width,CV_32F,1);
	}else{
		this->bgProbability = this->getBackground(prevFrames->end(),currFrameImage);
	}
}