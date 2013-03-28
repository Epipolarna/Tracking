#include "ProbabilityMap.h"

ProbabilityMap::ProbabilityMap(Frame* currFrame){
	
	this->models = 0;
	pImage = currFrame->image;
}


ProbabilityMap::ProbabilityMap(Frame *prevFrame, Frame *currFrame){	
	
	static int counter = 0;

	if(prevFrame == NULL){

		this->models = new PixelModel(currFrame->image.rows, currFrame->image.cols);
	}else{

		this->models = prevFrame->bgModel.models;
		if(counter < 50){
			this->models->updateModel(currFrame->image);
		}
	}

	pImage = models->getPmat(currFrame->image);
	counter = counter + 1;
}