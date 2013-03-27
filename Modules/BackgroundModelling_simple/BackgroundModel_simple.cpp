#include "BackgroundModel_simple.h"


namespace BackgroundModelling_simple
{
	void BackgroundModel::update(std::list<Frame> &frames)
	{
		Frame * current = &frames.front();
		Frame * previous = NULL;
		
		
		if(frames.size() <= 1)
		{
			current->bgModel = ProbabilityMap(current);
			probabilityMap = Mat::zeros(current->image.size(), CV_8UC3);
			blurredImage = Mat::zeros(current->image.size(), CV_8UC3);
		}
		else
		{
			current->bgModel = ProbabilityMap(current);
		}
		//for ( int i = 1; i < MAX_KERNEL_LENGTH; i = i + 2 ) // Better smoothing..
		int i = MAX_KERNEL_LENGTH;
			blur( current->image, blurredImage, Size( i, i ), Point(-1,-1) );
			//GaussianBlur( current->image, blurredImage, Size( i, i ), 0,0 );
		
		float learningRate = 100;

		if(frames.size() < 6)
			probabilityMap += blurredImage.clone()/5;
		else if(counter == 0)
		{
			probabilityMap = blurredImage.clone()/learningRate + (learningRate-1)/learningRate*probabilityMap;
		}
		counter = (counter+1) % 8;

		
		/*	// Do not threshold
		current->bgModel.pImage = Mat::zeros(probabilityMap.size(), CV_8UC1);
		for(int row = 4; row < probabilityMap.size().height; row += 3)
			for(int col = 4; col < probabilityMap.size().width; col += 3)
				current->bgModel.pImage.at<uchar>(row, col) = (std::abs(probabilityMap.at<Vec3b>(row, col)[0] - current->image.at<Vec3b>(row, col)[0]) + 
													    std::abs(probabilityMap.at<Vec3b>(row, col)[1] - current->image.at<Vec3b>(row, col)[1]) + 
													    std::abs(probabilityMap.at<Vec3b>(row, col)[2] - current->image.at<Vec3b>(row, col)[2]));

			*/
		current->bgModel.estimatedBackground = probabilityMap;
		current->bgModel.pImage = Mat::zeros(probabilityMap.size(), CV_8UC1);
		for(int row = 4; row < probabilityMap.size().height-1; row += 3)
			for(int col = 4; col < probabilityMap.size().width-1; col += 3)
			{
				for(int i = 0; i < 3; i++)
					if(std::abs(probabilityMap.at<Vec3b>(row, col)[i] - blurredImage.at<Vec3b>(row, col)[i]) > varianceTreshold)
					{				
						 // Optimal box for row,col += 3
						current->bgModel.pImage.at<uchar>(row-2, col-2) = 255;
						current->bgModel.pImage.at<uchar>(row-2, col-1) = 255;						
						current->bgModel.pImage.at<uchar>(row+1, col-2) = 255;
						current->bgModel.pImage.at<uchar>(row, col-2) = 255;
						current->bgModel.pImage.at<uchar>(row-2, col+1) = 255;
						current->bgModel.pImage.at<uchar>(row-1, col+1) = 255;
						current->bgModel.pImage.at<uchar>(row+1, col) = 255;
						current->bgModel.pImage.at<uchar>(row+1, col+1) = 255;
						
						
						/* // Optimal box for row,col += 2
						current->bgModel.pImage.at<uchar>(row-1, col-1) = 255;
						current->bgModel.pImage.at<uchar>(row+1, col-1) = 255;
						current->bgModel.pImage.at<uchar>(row-1, col+1) = 255;
						current->bgModel.pImage.at<uchar>(row+1, col+1) = 255;
						*/
						break;
					}

			}

		current->backgroundProbMap = current->bgModel.pImage.clone();

	}

	void BackgroundModel::display(std::string windowID)
	{
		imshow( windowID.c_str(), probabilityMap );
	}
}