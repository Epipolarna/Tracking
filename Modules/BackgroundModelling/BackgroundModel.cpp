//
//  BackgroundModel.cpp
//  Gamma
//
//  Created by Gustav Häger on 2013-03-29.
//  Copyright (c) 2013 Gustav Häger. All rights reserved.
//

#include "BackgroundModel.h"
#include <iostream>
using namespace std;

namespace BackgroundModelling
{
    
    BackgroundModel::BackgroundModel(float alpha, float minSigma, float maxW, float lambda) : alpha(alpha), minSigma(minSigma), maxW(maxW),lambda(lambda){
        bgThresh = 0.8;
        namedWindow("debug");
    }
    
	void BackgroundModel::update(std::list<Frame> &frames){
		Frame * currFrame = &frames.front();
		Frame * prevFrame;
		
		if(frames.size() > 1){
 			prevFrame = &(*(++frames.begin()));
            _updateModel(currFrame);
		}else{

			_initalizeModel(currFrame->image);
		}
	}
    
    Mat BackgroundModel::getBackground(Mat image){
        float distance;
        for(int row = 0; row < numRows; row++){
            for(int col = 0; col < numCols; col++){
                for(int k = 0; k < NUM_GAUSS; k++){

                    distance = _computeDistance(&p[row*numCols + col].models[k],
                                                image.at<Vec3b>(row,col));
                    
                    if(distance < lambda && p[row*numCols + col].models[k].background == true){
                        bgModel.at<uchar>(row,col) = 255;
                    }else{
                        bgModel.at<uchar>(row,col) = 0;
                    }
                }
            }
        }
        return bgModel;
    }
    
    Mat BackgroundModel::getProbableBackground(){
        return probableBackgrund;
    }
    
    PixelModel* BackgroundModel::_getModel(int row, int col, int k){
        return &p[row*numCols + col].models[k];
    }
    
    void BackgroundModel::_updateModel(Frame *currFrame){
        Mat floatImage;
        currFrame->image.convertTo(floatImage, CV_32FC3);
        _updateDistributions(floatImage);
        _buildBackgroundMatrix();
    }
    
    void BackgroundModel::_updateDistributions(Mat image){
        for(int row = 0; row < numRows; row++){
            uchar *imagePointer = image.ptr(row);
            
            for(int col = 0; col < numCols*3; ++col){
                Vec3b colors;
                colors[0] = *imagePointer++;
                colors[1] = *imagePointer++;
                colors[2] = *imagePointer++;
                _updatePixel(&p[row*numCols + col],colors);
            }
        }
    }
    
    float BackgroundModel::_computeSigmaNorm(PixelModel *pm){
        float norm = 0;
        
        for(int n=0; n < NUM_CHANNELS; n++){
            norm = norm + pow(pm->gauss[n].sigma,2);
        }
        return sqrt(norm);
    }
    
    void BackgroundModel::_updatePixel(Pixel *p, Vec3b colors){
        int bestK = -1;
        float distance;
        float bestDistance;
        
        //get the closest distribution
        for(int k=0; k < NUM_GAUSS; k++){
            distance = _computeDistance(&p->models[k], colors);
            
            if(distance < lambda){
                if(bestK == -1){
                    bestK = k;
                    bestDistance = distance;
                }else if(bestDistance/p->models[bestK].w < distance/p->models[k].w){
                    bestK = k;
                    bestDistance = distance;
                }
            }
        }
        //update the closest one or replace the worst one
        if(bestK == -1){
            //did not match at all, replace least used distribution
            _initModel(&p->models[NUM_GAUSS - 1], colors);
        }else{
            //update the matched one
            _updateModel(&p->models[bestK],colors);
            _normalizePixelModels(p);
            _sortPixelModels(p);
            _markBackground(p);
        }
    }
    
    void BackgroundModel::_markBackground(Pixel *p){
        float acc = 0;
        int k = 0;
        while(acc < bgThresh && k < NUM_GAUSS){
            acc = acc + p->models[k].w;
            k = k + 1;
            p->models[k].background = true;
        }
        
        for(; k < NUM_GAUSS; k++){
            p->models[k].background = false;
        }
    }
    
    void BackgroundModel::_buildBackgroundMatrix(){
        //make a most-likely background
        for(int row = 0; row < numRows; row++){
            uchar *imagePointer = probableBackgrund.ptr(row);
            for(int col = 0; col < numCols*3; ++col){
                for(int n = 0; n < NUM_CHANNELS; n++){
                    *imagePointer = p->models[0].gauss[n].mean;
                    imagePointer++;
                }
            }
        }
    }
    
    void BackgroundModel::_swapModels(PixelModel *p1, PixelModel *p2){
        PixelModel temp;
        
        temp.w = p1->w;
        p1->w = p2->w;
        p2->w = temp.w;
        
        for(int n = 0; n < NUM_CHANNELS; n++){
            temp.gauss[n] = p1->gauss[n];
            p1->gauss[n] = p2->gauss[n];
            p2->gauss[n] = temp.gauss[n];
        }
    }
    
    bool BackgroundModel::_compareModels(PixelModel *p1, PixelModel *p2){
        float norm1 = _computeSigmaNorm(p1);
        float norm2 = _computeSigmaNorm(p2);
        return sqrt(norm1) < sqrt(norm2);
    }
    
    void BackgroundModel::_sortPixelModels(Pixel *p){
        
        for(int i=0; i < NUM_GAUSS; i++){
            for(int j=0; j < NUM_GAUSS; j++){
                if(_compareModels(&p->models[i],&p->models[j])){
                    _swapModels(&p->models[i],&p->models[j]);
                }
            }
        }
    }
    
    void BackgroundModel::_normalizePixelModels(Pixel *p){
        float normFactor = 0;
        for(int k = 0; k < NUM_GAUSS; k++){
            normFactor = normFactor + p->models[k].w;
        }
        
        for(int k = 0; k < NUM_GAUSS; k++){
            p->models[k].w = p->models[k].w / normFactor;
        }
    }
    
    void BackgroundModel::_updateModel(PixelModel *p, Vec3b colors){
        p->w = (1 - alpha)*p->w + alpha;
        float roh = alpha / p->w;
        
        if(p->w > maxW){
            p->w = maxW;
        }
        
        for(int n=0; n < NUM_CHANNELS; n++){
            p->gauss[n].mean = (1 - roh) * p->gauss[n].mean + roh * colors[n];
            p->gauss[n].sigma = (1 - roh) * p->gauss[n].sigma + roh*pow(colors[n] - p->gauss[n].mean,2);
            
            if(p->gauss[n].sigma < minSigma){
                p->gauss[n].sigma = minSigma;
            }
        }
    }
    
    float BackgroundModel::_computeDistance(PixelModel *pm, Vec3b color){
        float distance = 0;
        for(int c=0; c < NUM_CHANNELS; c++){
            distance = distance + pow(color[c] - pm->gauss[c].mean,2)/pm->gauss[c].sigma;
        }
        return distance;
    }
    
    //init code
    void BackgroundModel::_initModel(PixelModel *pm, const Vec3b initMean){
        pm->w = 1.0 / NUM_GAUSS;
        for(int n=0; n < NUM_CHANNELS; n++){
            pm->gauss[n].mean = initMean[n];
            pm->gauss[n].sigma = 20;
        }
    }
    
    void BackgroundModel::_initPixel(Pixel *p, const Vec3b initMean){
        for(int k=0; k < NUM_GAUSS; k++){
            _initModel(&p->models[k],initMean);
        }
    }
    
    void BackgroundModel::_initalizeModel(Mat image){
        numRows = image.rows;
        numCols = image.cols;
        p = new Pixel[numRows * numCols];
        
        for(int row = 0; row < numRows; row++){
            for(int col = 0; col < numCols; col++){
                _initPixel(&p[row*numCols+col],image.at<Vec3b>(row,col));
            }
        }
    }
}