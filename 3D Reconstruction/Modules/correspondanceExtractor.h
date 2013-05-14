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
#include <vector>

struct pointPair
{
	cv::Point2d p1, p2;
	pointPair(){}
	pointPair(cv::Point2d p1, cv::Point2d p2): p1(p1),p2(p2) {}
};


// Chain generation
struct ChainNode
{
	cv::Point2d point;
	ChainNode(cv::Point2d point):point(point){}
};


class CorrespondanceExtractor
{
public:
	CorrespondanceExtractor();
	void init(std::string dataSet);
	void readImages(std::string prefix, int numberOfImages, std::string postfix);
	void findMatches();
	void saveMatches(std::string filename);
	bool loadMatches(std::string filename);

	void getBAPoints(int imagePair, std::vector<cv::Point2d>& BAPoints1, std::vector<cv::Point2d>& BAPoints2);

	void generateChains();

public:
	cv::Mat K;
	std::vector<cv::Mat> imageList;
	std::vector< std::vector<pointPair> > matchesVector;

	// Chain generation
	std::vector<std::vector<ChainNode>> chain;
	std::vector<int> startOnImage;
	

};


#endif