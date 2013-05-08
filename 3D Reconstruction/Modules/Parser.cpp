#include "Parser.h"


bool Parser::parseMatrix(std::ifstream & file, cv::Mat & destination, int type)
{
	std::string firstLine;
	std::getline(file, firstLine);
	while(line.find('[') == std::string::npos)
		std::getline(file, firstLine);

	return parseMatrix(file, firstLine, destination, type);
}

bool Parser::parseMatrix(std::ifstream & file, std::string & firstLine, cv::Mat & destination, int type)
{
	line = firstLine.substr(firstLine.find('[')+1);
	do
	{
		matrixWidth = 0;
		ss = std::istringstream(line);
		while(!ss.fail())
		{					
			ss >> value;
			ss >> tmp;
			if(!ss.fail())
			{
				matrixWidth++;
				matrixData.push_back(value);
			}
		}
	} while(line.find(']') == std::string::npos && std::getline(file, line));
	destination = cv::Mat(matrixData.size()/matrixWidth,matrixWidth,type,&matrixData[0]);
	return true;
}
