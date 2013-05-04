/* TSBB15 3D Reconstruction project 2013
 * - - - - - - - - - - - - - - -
 * Principal contributer: Alexander Sjöholm
 */

#ifndef _CORRESPONDACEEXTRACTOR_H_
#define _CORRESPONDACEEXTRACTOR_H_

#include <opencv\cvaux.h>
#include <opencv\cv.h>
#include <opencv\highgui.h>
#include <opencv2\nonfree\features2d.hpp>
#include <fstream>
#include <string>

struct pointPair
{
	cv::Point2d p1, p2;
	pointPair(){}
	pointPair(cv::Point2d p1, cv::Point2d p2): p1(p1),p2(p2) {}
};

void readImages(std::vector<cv::Mat>* imageList);
void findMatches(std::vector<cv::Mat>* imageList, std::vector<std::vector<pointPair>>* matchesVector);
void saveMatches(std::vector<std::vector<pointPair>>* matchesVector, char* filename);
void loadMatches(std::vector<std::vector<pointPair>>* matchesVector, char* filename);


#endif