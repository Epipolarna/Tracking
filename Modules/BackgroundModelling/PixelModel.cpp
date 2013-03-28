#include "PixelModel.h"

PixelModel::PixelModel(int maxRow, int maxCol){

    //Parameters, tunable
	lambda = 10;
	alpha = 0.05f;
	backgroundThreshold = 0.75;
    minSigma = 10;
    maxW = 0.8;
    
    
    rows = maxRow;
	cols = maxCol;
	dists = new PixelModelData[maxRow * maxCol];
	initModel();
}

void PixelModel::initModel(){
	
	//TODO fix so we initialize with some data
	double pixelValues[NUM_CHANNELS] = {50,50,50};
	double sigma[NUM_CHANNELS] = {30,30,30};

	for(int r=0; r < rows; r++){
		for(int c=0; c < cols; c++){
			for(int k=0; k < NUM_DISTRIBUTIONS; k++){
				initGauss(getPixelGauss(r,c,k),sigma,pixelValues,1.0/NUM_DISTRIBUTIONS);
				for(int i=0; i < NUM_CHANNELS; i++){
					pixelValues[i] = 50 * k + 50;
				}
			}

		}
	}
}

Gauss* PixelModel::getPixelGauss(int row, int col, int k){
	assert(k < NUM_DISTRIBUTIONS);
	assert(k >= 0);

	return &dists[row * cols + col].models[k];
}

Gauss* PixelModel::getPixelGauss(int row, int col){
	assert(row < rows);
	assert(col < cols);
	assert(row >= 0);
	assert(col >= 0);
	Gauss *g = &dists[row*cols + col].models[0];
	return g;
}

void PixelModel::initGauss(Gauss *g, double *sigma, double *mean, double w){
	static int counter = 0;
	g->w = w;
	g->isBackground = false;

	for(int c=0; c < NUM_CHANNELS; c++){
		g->mean[c] = mean[c];
		g->sigma[c] = sigma[c];
		g->distributionID = counter;
	}
	g->wasUpdated = 10;
	counter = counter + 1;
}

double PixelModel::sigmaSize(int row, int col, int k){
	Gauss *g = getPixelGauss(row,col,k);
	double number = 0;
	for(int c=0; c < NUM_CHANNELS; c++){
		number = number + g->sigma[c] * g->sigma[c];
	}
	return sqrt(number);
}



void PixelModel::updatePixelGauss(int row,int col,int k, Mat image){
	Gauss *g;
	double roh;
	double fSig;
	double fMean;
	static int tio = 10;

	if(k == -1){
		//we need to create a new distribution here in place of the worst one
		 g = getPixelGauss(row,col,NUM_DISTRIBUTIONS-1);
		 
		 for(int c=0; c < NUM_CHANNELS; c++){
		 	g->mean[c] = image.at<Vec3b>(row,col)[c];
		 	g->sigma[c] = 30;
		 }
		 g->w = 0.1;
		 g->isBackground = false;
		 g->wasUpdated = 10;

	}else{

		g = getPixelGauss(row,col,k);
		g->wasUpdated = 11;
		g->w = (1 - alpha)*g->w + alpha;
		roh = alpha / g->w;

		if(g->w > maxW){
			g->w = maxW;
		}


		//update all channels in the gauss
		for(int c=0; c < NUM_CHANNELS; c++){
			roh = alpha/g->w;
			
			fMean = (1 - roh)*g->mean[c] + roh*image.at<Vec3b>(row,col)[c];
			fSig = (1 - roh)*g->sigma[c] + roh*(image.at<Vec3b>(row,col)[c] - g->mean[c]) * (image.at<Vec3b>(row,col)[c] - g->mean[c]);

			g->mean[c] = fMean;
			g->sigma[c] = fSig;

			if(g->sigma[c] < minSigma){
				g->sigma[c] = minSigma;
			}
		}
	}
}

void PixelModel::normalisePixelGausses(int r, int c){
	double normFactor = 0;
	Gauss *g;
	for(int k=0; k < NUM_DISTRIBUTIONS; k++){
		g = getPixelGauss(r,c,k);
		normFactor = normFactor + g->w;
	}
	for(int k=0; k < NUM_DISTRIBUTIONS; k++){
		g = getPixelGauss(r,c,k);
		g->w = g->w / normFactor;
	}

}

void PixelModel::updatePixel(int r, int c,Mat image){
	double distances[NUM_DISTRIBUTIONS];
	int bestMatch = -1;
	//compute the best matching distribution for the current
	//pixel value
	for(int k=0; k < NUM_DISTRIBUTIONS; k++){
		
		cv::Vec3b pixel = image.at<Vec3b>(r,c);
		distances[k] = distanceToGauss(getPixelGauss(r,c,k),pixel);

		if(distances[k] < lambda){
			if(bestMatch == -1){
				bestMatch = k;
			}else if(getPixelGauss(r,c,k)->w/sigmaSize(r,c,k) > 
							 getPixelGauss(r,c,bestMatch)->w/sigmaSize(r,c,bestMatch)){
				bestMatch = k;
			}
		}
	}
	//update and normalize the pixels
	updatePixelGauss(r,c,bestMatch,image);
	normalisePixelGausses(r,c);
	sortPixelGauss(r,c);
	markBackground(r,c);
}

void PixelModel::markBackground(int row,int col){
	Gauss *g;
	double ff = 0;
	int k = 0;
	while(ff < backgroundThreshold && k < NUM_DISTRIBUTIONS){
		g = getPixelGauss(row,col,k);
		ff = ff + g->w;
		g->isBackground = true;
		k++;
	}
	k++;
	for(;k < NUM_DISTRIBUTIONS; k++){
		g = getPixelGauss(row,col,k);
		g->isBackground = false;
	}
}

void PixelModel::updateModel(Mat image){
	for(int r=0; r < rows; r++){
		for(int c=0; c < cols; c++){
			updatePixel(r,c,image);
		}
	}
}

double PixelModel::distanceToGauss(Gauss *g, cv::Vec3b pixel){
	double distance = 0;
	for(int c=0; c < NUM_CHANNELS; c++){
		distance = distance + (pixel[c] - g->mean[c])*(pixel[c] - g->mean[c])/g->sigma[c];
	}
	return distance;
}

bool PixelModel::compare(Gauss *g1, Gauss *g2){
	int norm1 = 0;
	int norm2 = 0;

	for(int c=0; c < NUM_CHANNELS;c++){
		norm1 = norm1 + int(g1->sigma[c] * g1->sigma[c]);
		norm2 = norm2 + int(g2->sigma[c] * g2->sigma[c]);
	}

	return (g1->w / sqrt(norm1)) > (g2->w / sqrt(norm2));
}

void PixelModel::copyGauss(Gauss *s, Gauss *d){
	for(int c=0; c < NUM_CHANNELS; c++){
		d->mean[c] = s->mean[c];
		d->sigma[c] = s->sigma[c];
	}
	d->distributionID = s->distributionID;
	d->w = s->w;
}

void PixelModel::sortPixelGauss(int row,int col){
	//TODO implement this
	Gauss *g1, *g2;
	Gauss temp;
	for(int i=0; i < NUM_DISTRIBUTIONS; i++){
		for(int j=0; j < NUM_DISTRIBUTIONS; j++){
			g1 = getPixelGauss(row,col,i);
			g2 = getPixelGauss(row,col,j);

			if(compare(g1,g2)){
				//swap
				copyGauss(g2,&temp);
				copyGauss(g1,g2);
				copyGauss(&temp,g1);
			}
		}
	}
}

void PixelModel::dumpPixel(int row,int col, Mat image){
	cout << "distance to this is: " << distanceToGauss(getPixelGauss(row,col,0),image.at<Vec3b>(row,col)) <<endl;
	cout << "w0 is: " << getPixelGauss(row,col,0)->w << endl; 
	cout << " sigma1: " << getPixelGauss(row,col,0)->sigma[0] << "   ";
	cout << " sigma2: " << getPixelGauss(row,col,0)->sigma[1] << "   ";
	cout << " sigma3: " << getPixelGauss(row,col,0)->sigma[2] << "   " <<endl;;
	cout << " mean1: " << getPixelGauss(row,col,0)->mean[0] << "   ";
	cout << " mean2: " << getPixelGauss(row,col,0)->mean[1] << "   ";
	cout << " mean3: " << getPixelGauss(row,col,0)->mean[2] << "   " <<endl;;
	cout << "is background: " << getPixelGauss(row,col,0)->isBackground << endl;
	cout << "id is: " << getPixelGauss(row,col,0)->distributionID <<endl;
	cout << "was this distribution updated: " << getPixelGauss(row,col,0)->wasUpdated << endl <<endl;

	cout << "distance to this is: " << distanceToGauss(getPixelGauss(row,col,1),image.at<Vec3b>(row,col)) <<endl;
	cout << "w1 is: " << getPixelGauss(row,col,1)->w << endl;
	cout << " sigma1: " << getPixelGauss(row,col,1)->sigma[0] << "   ";
	cout << " sigma2: " << getPixelGauss(row,col,1)->sigma[1] << "   ";
	cout << " sigma3: " << getPixelGauss(row,col,1)->sigma[2] << "   " <<endl;;
	cout << " mean1: " << getPixelGauss(row,col,1)->mean[0] << "   ";
	cout << " mean2: " << getPixelGauss(row,col,1)->mean[1] << "   ";
	cout << " mean3: " << getPixelGauss(row,col,1)->mean[2] << "   " <<endl;;
	cout << "is background: " << getPixelGauss(row,col,1)->isBackground << endl;
	cout << "id is: " << getPixelGauss(row,col,1)->distributionID <<endl;
	cout << "was this distribution updated: " << getPixelGauss(row,col,1)->wasUpdated << endl<<endl;
	
	cout << "distance to this is: " << distanceToGauss(getPixelGauss(row,col,2),image.at<Vec3b>(row,col)) <<endl;
	cout << "w2 is: " << getPixelGauss(row,col,2)->w << endl;
	cout << " sigma1: " << getPixelGauss(row,col,2)->sigma[0] << "   ";
	cout << " sigma2: " << getPixelGauss(row,col,2)->sigma[1] << "   ";
	cout << " sigma3: " << getPixelGauss(row,col,2)->sigma[2] << "   " <<endl;;
	cout << " mean1: " << getPixelGauss(row,col,2)->mean[0] << "   ";
	cout << " mean2: " << getPixelGauss(row,col,2)->mean[1] << "   ";
	cout << " mean3: " << getPixelGauss(row,col,2)->mean[2] << "   " <<endl;;
	cout << "is background: " << getPixelGauss(row,col,2)->isBackground << endl;
	cout << "id is: " << getPixelGauss(row,col,2)->distributionID <<endl;
	cout << "was this distribution updated: " << getPixelGauss(row,col,2)->wasUpdated << endl <<endl;
	cout << "-------------------------" <<endl;	
}

Mat PixelModel::getPmat(Mat image){
	Mat pData = Mat(rows,cols,CV_64F);
	Gauss *g;

	for(int r=0; r < rows; r++){
		for(int c=0; c < cols; c++){
			
			double pixelProb = 0;
			for(int k=0; k < NUM_DISTRIBUTIONS; k++){
				g = getPixelGauss(r,c,k);
				g->wasUpdated = 9;
				pixelProb = pixelProb + distanceToGauss(g,image.at<Vec3b>(r,c));
				
				if(g->isBackground && (sqrt(pixelProb) < lambda)){
					pData.at<double>(r,c) = 0.1;	
					break;
				}else{
					pData.at<double>(r,c) = 0.9;	
				}

			}
		}
	}
	return pData;
}