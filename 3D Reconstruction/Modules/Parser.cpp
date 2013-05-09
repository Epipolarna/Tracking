#include "Parser.h"


bool Parser::parseMatrix(std::ifstream & file, cv::Mat & destination, int type)
{
	std::string firstLine;
	std::getline(file, firstLine);
	while(firstLine.find('[') == std::string::npos)
		std::getline(file, firstLine);

	return parseMatrix(file, firstLine, destination, type);
}

bool Parser::parseMatrix(std::ifstream & file, std::string & firstLine, cv::Mat & destination, int type)
{
	line = firstLine.substr(firstLine.find('[')+1);
	matrixData.clear();
	bool widthDetected = false;
	matrixWidth = 0;
	do
	{
		//ss = std::istringstream(line);
		std::stringstream ss;
		ss.precision(20);
		ss << line;
		while(!ss.fail())
		{					
			ss >> value;
			ss >> tmp;
			if(!ss.fail())
			{
				if(!widthDetected)
				{
					matrixWidth++;
					if(tmp == ';')
						widthDetected = true;
				}
				matrixData.push_back(value);
			}
		}
	} while(line.find(']') == std::string::npos && std::getline(file, line));
	if(matrixData.size() > 0)
	{
		double * matrixArray = new double[matrixData.size()];
		memcpy(matrixArray, &matrixData[0], matrixData.size()*sizeof(matrixData[0]));
		destination = cv::Mat(matrixData.size()/matrixWidth,matrixWidth,type,matrixArray);
	}
	return true;
}

bool Parser::parsePoint2d(std::string & line, cv::Point2d & p2d)
{
	std::stringstream ss;
	ss.precision(20);
	ss << line.substr(line.find('[')+1);
	ss >> p2d.x;
	ss >> tmp;
	ss >> p2d.y;
	return !ss.fail();
}

bool Parser::parsePoint3d(std::string & line, cv::Point3d & p3d)
{
	std::stringstream ss;
	ss.precision(20);
	ss << line.substr(line.find('[')+1);
	ss >> p3d.x;
	ss >> tmp;
	ss >> p3d.y;
	ss >> tmp;
	ss >> p3d.z;
	return !ss.fail();
}
