#include "ProbabilityMap.h"

ProbabilityMap::ProbabilityMap(Frame *prevFrame, Frame *currFrame){	
	
	numGauss = 3;
	lambda = 2.5; //this should probably be tweaked to be lower
	initSigma = 3;
	int numPixels = currFrame->image.rows * currFrame->image.cols;
	biggestW = new float[numPixels];
	Mat image = currFrame->image;
	
	if(prevFrame == NULL){

		int cRow = 0,cCol = 0;
		distributions = new gauss3D[numGauss*numPixels];
		//for all pixels
		for(int row=0; row < image.rows; row++){
			//for all columns
			for(int col=0; col < image.cols*3; col++){
				//for each channel in the image
				for(int c=0; c < 3; c++){
					//set all gausess
					for(int k=0; k < numGauss; k++){
						distributions[row*image.rows+col+k].mean[c] = image.at<Vec3b>(row,col)[c];
						distributions[row*image.rows+col+k].sigma[c] = initSigma;
						distributions[row*image.rows+col+k].mean[c] = 1.0/float(numGauss);
					}
				}
			}
		}
	}else{
		this->distributions = prevFrame->pData->distributions;
		updateDistributions(image);
	}
	setB(image.rows,image.cols);
}

void ProbabilityMap::updateDistributions(Mat image){
	int bestMatch;
	float d[numGauss];

	//update all distributions for all pixels
	for(int row=0; row < image.rows; row++){
		for(int col=0; col < image.cols; col++){
			//measure distance to each gaussian for this pixel
			resetDistance(d);
			bestMatch = -1;
			gauss3D bestDist;
			for(int k=0; k < numGauss; k++){
				gauss3D distK = distributions[row*image.rows+col+k];
				//for each channel
				for(int c=0; c < 3; c++){
					int a =	image.at<Vec3b>(row,col)[c];
					int b = distK.mean[c];
					float sigma = distK.sigma[c];
					d[k] = d[k] + pow((a-b)/sigma,2);
				}

				if(d[k] < lambda){
					if(bestMatch == -1){
						bestDist = distK;
					}else if(betterMatch(bestDist,distK)){
						bestDist = distK;
						bestMatch = k;
					}
				}
			}

			if(bestMatch == -1){//create a new distribution if we did not match at all
				bestMatch = numGauss;//this should be the worst match instead
				initDistribution(distributions[row*image.rows+col+bestMatch],
												 initSigma,image.at<Vec3b>(row,col));
			}else{
				float roh;
				float w = distributions[row*image.rows+col+bestMatch].w;
				distributions[row*image.rows+col+bestMatch].w = w = (1 - alpha)*w + alpha;
				roh = alpha/w;
				for(int c=0; c < 3; c++){
					distributions[row*image.rows+col+bestMatch].mean[c] = (1 - roh)*distributions[row*image.rows+col+bestMatch].mean[c] + roh*image.at<Vec3b>(row,col)[c];
					distributions[row*image.rows+col+bestMatch].sigma[c] = (1 - roh)*distributions[row*image.rows+col+bestMatch].sigma[c] + roh*pow(image.at<Vec3b>(row,col)[c] - distributions[row*image.rows+col+bestMatch].mean[c],2);
				}
			}
			float wSum = sumW(row,col,image.rows);
			for(int k=0; k < numGauss; k++){
				float w = distributions[row*image.rows+col+k].w;
				distributions[row*image.rows+col+k].w = w/wSum;
				float wSig = distributions[row*image.rows+col+k].w / sigmaSize(distributions[row*image.rows+col+k]);
				if(biggestW[row*image.rows+col+k] < wSig){
					biggestW[row*image.rows+col+k] = wSig;
				}
			}
		}
	}
}

void ProbabilityMap::setB(int rows, int cols){
	Mat p(rows,cols,CV_32FC2);

	for(int row=0; row < rows; row++){
		for(int col=0; col < cols; col++){
			p.at<float>(row,col) = biggestW[row*rows+col];
		}
	}
	pImage = p;
}

float ProbabilityMap::sumW(int row, int col,int maxRow){
	float acc = 0;
	for(int k=0; k < numGauss; k++){
		acc = acc + distributions[row*maxRow+col+k].w;
	}
	return acc;
}

void ProbabilityMap::initDistribution(gauss3D &g, uchar sigma, Vec3b mean){
	for(int i=0; i < 3; i++){
		g.sigma[i] = sigma;
		g.mean[i] = mean[i];
	}
	g.w = 1/numGauss;
}

float ProbabilityMap::sigmaSize(gauss3D g){
	float acc = 0;
	for(int i=0; i < 3; i++){
		acc = acc + sqrt(g.sigma[i]);
	}
	return acc;
}

bool ProbabilityMap::betterMatch(gauss3D bestMatch, gauss3D thisone){
	return thisone.w/sigmaSize(thisone) > bestMatch.w/sigmaSize(bestMatch);
}

void ProbabilityMap::resetDistance(float *d){
	for(int i=0; i < numGauss; i++){
		for(int c=0; c < 3; c++){
			d[i] = 0;
		}
	}
}