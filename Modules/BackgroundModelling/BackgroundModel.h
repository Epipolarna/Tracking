//
//  BackgroundModel.h
//  Gamma
//
//  Created by Gustav Häger on 2013-03-29.
//  Copyright (c) 2013 Gustav Häger. All rights reserved.
//

#ifndef __Gamma__BackgroundModel__
#define __Gamma__BackgroundModel__

#include "Frame.h"

#include <list>
#include <opencv2/core/core.hpp>

#define NUM_GAUSS 3
#define NUM_CHANNELS 3

class Frame;

using namespace cv;

namespace BackgroundModelling
{
    struct Gaussian{
        float mean;
        float sigma;
    };
    
    struct PixelModel{
        float w;
        bool background;
        Gaussian gauss[NUM_CHANNELS];
    };
    
    struct Pixel{
        PixelModel models[NUM_GAUSS];
    };
    
	class BackgroundModel{
        
	public:
        BackgroundModel(float alpha, float minSigma,float maxW, float lambda);
		void update(std::list<Frame> &frames);
        Mat getBackground(Mat image);
        Mat getProbableBackground();
        
    private:
        //tuning parameters
        float alpha, minSigma, maxW, lambda;
        float bgThresh;
        int skipRate;
        Pixel *p;
        
        //data
        Mat bgModel;
        Mat probableBackgrund;
        int numRows, numCols;
        
        //private methods
        
        //updating
        void _updateModel(Frame *currFrame);
        void _updateDistributions(Mat image);
        void _updatePixel(Pixel *p, Vec3b colors);
        void _updateModel(PixelModel *p, Vec3b colors);
        void _normalizePixelModels(Pixel *p);
        void _sortPixelModels(Pixel *p);
        void _buildBackgroundMatrix();
        void _markBackground(Pixel *p);
        
        //helper stuff for the sorting
        void _swapModels(PixelModel *p1, PixelModel *p2);
        bool _compareModels(PixelModel *p1, PixelModel *p2);
        
        //fetching, might not be needed...
        PixelModel* _getModel(int row, int col, int k);
        
        //size and distance
        float _computeDistance(PixelModel *pm, Vec3b color);
        float _computeSigmaNorm(PixelModel *pm);

        //initializing
        void _initalizeModel(Mat image);
        void _initPixel(Pixel *p, const Vec3b initMean);
        void _initModel(PixelModel *pm, const Vec3b initMean);
	};
}


#endif /* defined(__Gamma__BackgroundModel__) */
