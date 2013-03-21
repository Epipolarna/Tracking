#include "BackgroundModel_simple.h"


namespace BackgroundModelling_simple
{
	void BackgroundModel::update(std::list<Frame> &frames)
	{
		Frame * current = &frames.front();
		Frame * previous;
		
		
		if(frames.size() <= 1)
		{
			probabilityMap = Mat::zeros(current->image.size(), CV_8UC3);
			probabilityMapTemp = Mat::zeros(current->image.size(), CV_8UC3);
		}
		
		//for ( int i = 1; i < MAX_KERNEL_LENGTH; i = i + 2 )
		int i = MAX_KERNEL_LENGTH;
			blur( current->image, current->image, Size( i, i ), Point(-1,-1) );
		
		float learningRate = 100;

		if(frames.size() < 6)
			probabilityMap += current->image.clone()/5;
		else if(counter == 0)
		{
			probabilityMap = current->image.clone()/learningRate + (learningRate-1)/learningRate*probabilityMap;
		}
		counter = (counter+1) % 8;

			/*
		switch(counter)
		{
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
			break;
		case 6:
			probabilityMap = probabilityMapTemp.clone();
			for ( int i = 1; i < MAX_KERNEL_LENGTH; i = i + 2 )
				blur( probabilityMap, probabilityMap, Size( i, i ), Point(-1,-1) );
			break;

		}
		counter = (counter+1) % 16;
		*/


		current->probMap = Mat::zeros(probabilityMap.size(), CV_8UC1);
		for(int row = 4; row < probabilityMap.size().height; row += 3)
			for(int col = 4; col < probabilityMap.size().width; col += 3)
			{
				for(int i = 0; i < 3; i++)
					if(std::abs(probabilityMap.at<Vec3b>(row, col)[i] - current->image.at<Vec3b>(row, col)[i]) > varianceTreshold)
					{
						current->probMap.at<uchar>(row-2, col-2) = 255;
						current->probMap.at<uchar>(row-2, col) = 255;
						current->probMap.at<uchar>(row, col-2) = 255;
						current->probMap.at<uchar>(row-1, col) = 255;
						current->probMap.at<uchar>(row, col-1) = 255;
						current->probMap.at<uchar>(row, col) = 255;
						break;
					}

			}


		
		/* // Funkar typ med varianceThreshold = 20 & MAX_KERNEL_LENGTH = 10
		if(frames.size() <= 1)
		{
			current->image.convertTo(probabilityMap, CV_8UC3);
			probabilityMap = probabilityMap/5;
		}
		else if(frames.size() < 6)
		{
			probabilityMap += current->image/5;
		}
		else if(frames.size() == 6)
		{
			for ( int i = 1; i < MAX_KERNEL_LENGTH; i = i + 2 )
				blur( probabilityMap, probabilityMap, Size( i, i ), Point(-1,-1) );
		}
		
		for ( int i = 1; i < MAX_KERNEL_LENGTH; i = i + 2 )
			blur( current->image, current->image, Size( i, i ), Point(-1,-1) );

		current->probMap = Mat::zeros(probabilityMap.size(), CV_8UC1);
			for(int row = 2; row < probabilityMap.size().height; row++)
				for(int col = 0; col < probabilityMap.size().width; col++)
				{
					for(int i = 0; i < 3; i++)
						if(std::abs(probabilityMap.at<Vec3b>(row, col)[i] - current->image.at<Vec3b>(row, col)[i]) > varianceTreshold)
						{
							current->probMap.at<uchar>(row, col) = 255;
							break;
						}

				}
		*/
	}

	void BackgroundModel::display(std::string windowID)
	{
		imshow( windowID.c_str(), probabilityMap );
	}
}