#include "ProbabilityMap.h"

ProbabilityMap::ProbabilityMap(Frame* currFrame){
	
	this->models = 0;
	pImage = currFrame->image;
}


ProbabilityMap::ProbabilityMap(Frame *prevFrame, Frame *currFrame){	
	


	if(prevFrame == NULL){

		this->models = new PixelModel(currFrame->image.rows, currFrame->image.cols);
	}else{

		this->models = prevFrame->bgModel.models;
		this->models->updateModel(currFrame->image);
	}

	pImage = models->getPmat(currFrame->image);

}