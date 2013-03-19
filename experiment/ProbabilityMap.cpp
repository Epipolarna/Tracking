#include "ProbabilityMap.h"

ProbabilityMap::ProbabilityMap(Frame *prevFrame, Frame *currFrame){
	
	
	numGauss = 3;
	lambda = 2.5; //this should probably be tweaked to be lower
	int numPixels = currFrame->image.rows * currFrame->image.cols;
	Mat image = currFrame->image;
	
	if(prevFrame == NULL){

		int cRow = 0,cCol = 0;
		distributions = new gauss3D[numGauss*numPixels];
		//for all pixels
		for(int row=0; row < image.rows; row++){
			uchar* p = image.ptr(row);
			//for all columns
			for(int col=0; col < image.cols*3; col++){
				//for each channel in the image
				for(int c=0; c < 3; c++){
					//set all gausess
					for(int k=0; k < numGauss; k++){
						distributions[row*image.rows+col+k].mean[c] = image.at<Vec3b>(row,col)[c];
						distributions[row*image.rows+col+k].sigma[c] = initSigma;
						distributions[row*image.rows+col+k].mean[c] = 1/numGauss;
					}
				}
			}
		}
	}else{
		updateDistributions(prevFrame,currFrame);
	}

}

void ProbabilityMap::updateDistributions(Frame *prevFrame, Frame *currFrame){

}