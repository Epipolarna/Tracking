#include "ProbabilityMap.h"

ProbabilityMap::ProbabilityMap(Frame *prevFrame, Frame *currFrame){
	k = 3;


	if(prevFrame != NULL){
		for(int i=0; i < 3; i++){
			sigma[i] = prevFrame->pData->sigma[i].clone();
			mean[i] = prevFrame->pData->mean[i].clone();
		}
		updateDistributions(currFrame);
	}else{
		for(int i=0; i < 3; i++){
			sigma[i] = 1;
			mean[i] = currFrame->image;
		}
	}

}

void ProbabilityMap::updateDistributions(Frame *currFrame){
	
}