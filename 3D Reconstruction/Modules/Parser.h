#ifndef _PARSER_H_
#define _PARSER_H_

#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <opencv\cv.h>

class Parser
{
public:
	std::vector<double> matrixData;
	int matrixWidth;
	std::string line;
	std::istringstream ss;
	float value;
	char tmp;
public:
	bool Parser::parseMatrix(std::ifstream & file, std::string & firstLine, cv::Mat & destination, int type = CV_64FC1);
	bool Parser::parseMatrix(std::ifstream & file, cv::Mat & destination, int type = CV_64FC1);
};

#endif