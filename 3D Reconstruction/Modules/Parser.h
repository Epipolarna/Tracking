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
	bool parseMatrix(std::ifstream & file, std::string & firstLine, cv::Mat & destination, int type = CV_64FC1);
	bool parseMatrix(std::ifstream & file, cv::Mat & destination, int type = CV_64FC1);
	template<typename T>
	bool parseVariable(std::ifstream & file, T & variable);
	template<typename T>
	bool parseVariable(std::string & line, T & variable);
	bool parsePoint2d(std::string & line, cv::Point2d & p2d);
	bool parsePoint3d(std::string & line, cv::Point3d & p3d);
};


	template<typename T>
	bool Parser::parseVariable(std::ifstream & file, T & variable)
	{
		std::getline(file, line);
		int pos = line.find("=");
		if(pos == std::string::npos)
			return false;
		ss = std::istringstream(line.substr(pos+1));
		ss >> variable;
		return !ss.fail();
	}
	
	template<typename T>
	bool Parser::parseVariable(std::string & line, T & variable)
	{
		int pos = line.find("=");
		if(pos == std::string::npos)
			return false;
		ss = std::istringstream(line.substr(pos+1));
		ss >> variable;
		return !ss.fail();
	}

#endif