#include "ProbabilityMap.h"

ProbabilityMap::ProbabilityMap(Frame *prevFrame, Frame *currFrame)
	: lambda(2.5), initSigma(3), alpha(0.1){	
	
	int numPixels = currFrame->image.rows * currFrame->image.cols;
	biggestW = new float[numPixels];
	Mat image = currFrame->image;
	maxRow = image.rows;
	maxCol = image.cols;
	maxIndex = maxCol * maxRow;

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
						distributions[ci(row,col,k)].mean[c] = image.at<Vec3b>(row,col)[c];
						distributions[ci(row,col,k)].sigma[c] = initSigma;
						distributions[ci(row,col,k)].w = 1.0/numGauss;

					}
				}
			}
		}
	}else{
		distributions = prevFrame->bgModel->distributions;
		updateDistributions(image);
	}
	setB(image.rows,image.cols);
}

void ProbabilityMap::updateDistributions(Mat image){
	int bestMatch;
	float distance;

	//update all distributions for all pixels
	for(int row=0; row < image.rows; row++){
		for(int col=0; col < image.cols; col++){

			//measure distance to each gaussian for this pixel
			distance = 0;
			bestMatch = -1;
			gauss3D bestDist;
			for(int k=0; k < numGauss; k++){

				distance = distanceToGauss(distributions[ci(row,col,k)],image.at<Vec3b>(row,col));

				if(distance < lambda){//se if we belong to this distribution
					if(bestMatch == -1){
						bestDist = distributions[ci(row,col,k)];
					}else if(betterMatch(bestDist,distributions[ci(row,col,k)])){
						bestDist = distributions[ci(row,col,k)];
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
				w = (1 - alpha)*w + alpha;
				distributions[ci(row,col,bestMatch)].w = w;
				roh = alpha/w;
				for(int c=0; c < 3; c++){
					distributions[ci(row,col,bestMatch)].mean[c] = (1 - roh)*distributions[ci(row,col,bestMatch)].mean[c] + roh*image.at<Vec3b>(row,col)[c];
					distributions[ci(row,col,bestMatch)].sigma[c] = (1 - roh)*distributions[ci(row,col,bestMatch)].sigma[c] + roh*pow(image.at<Vec3b>(row,col)[c] - distributions[row*image.rows+col+bestMatch].mean[c],2);
				}
			}
			//något är fuckat här.
			//kolla mer imorgon bitti
			float wSum = sumW(row,col,image.rows);
			for(int k=0; k < numGauss; k++){
				float w = distributions[ci(row,col,k)].w;
				distributions[ci(row,col,k)].w = w/wSum;
				float wSig = distributions[ci(row,col,k)].w / sigmaSize(distributions[row*image.rows+col+k]);
				if(biggestW[ci(row,col)] < wSig){
					biggestW[ci(row,col)] = wSig;
				}
			}
		}
	}
}

uchar ProbabilityMap::distanceToGauss(gauss3D g, Vec<unsigned char, 3> p){
	uchar distance = 0;
	//for each channel
	for(int c=0; c < 3; c++){
		distance = distance + pow((p[c]-g.mean[c])/g.sigma[c],2);
	}
	return distance;
}

void ProbabilityMap::setB(int rows, int cols){
	Mat p(rows,cols,DataType<float>::type);

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

int ProbabilityMap::ci(int row, int col, int k){
	return ci(row, col) + k;
}

int ProbabilityMap::ci(int row, int col){
	return row * maxRow + col;
}