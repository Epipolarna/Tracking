#ifndef _EALUATION_H_
#define _EALUATION_H_

#include "../FrameList.h"
#include <vector>

class Evaluation
{
public:
	Evaluation(int threshold = 2);

	void readXML2FrameList(char* fileName);
	void currentFrame();

	vector<vector<Object>> frameList;
	void MOTP();
	void MOTA();

private:
	int frameCounter;
	int numberOfFrames, T;
	vector<int> numberOfObjects, matches, misses, flasePositive, mismatches;
	vector<vector<float>> distance;
	vector<map<int, int>> correspondance;

};

#endif
